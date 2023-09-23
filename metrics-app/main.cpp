#include "MetricsCore.h"
#include "yaml-cpp/yaml.h"
#include <atomic>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <prometheus/counter.h>
#include <prometheus/gauge.h>
#include <prometheus/histogram.h>
#include <prometheus/summary.h>
#include <signal.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <unordered_map>

static std::atomic_bool running = true;

void sig_handler(int) {
    std::cout << "Shutting down container\n";
    running = false;
}

void usage() {
    std::cerr << "Usage\n"
              << "metrics-app [-f <configFile>][-l <logLevel>]\n";
}

uint32_t getContainerInstance() {
    uint32_t result = 0;
    const char *hostnameEnv = getenv("HOSTNAME");
    if (hostnameEnv) {
        std::string hostname(hostnameEnv);

        // Expect stateful set hostnames to end in -<instance>
        size_t pos = hostname.find_last_of('-');
        if (pos != std::string::npos) {
            std::string instance = hostname.substr(pos + 1);
            try {
                result = static_cast<uint32_t>(std::stoul(instance, nullptr, 10));
            } catch (...) {
                // Contain exception
            }
        }
    }
    return result;
}

int main(int argc, char **argv) {
    int logLevel = spdlog::level::trace;
    std::string configFile = "metrics-app.yaml";
    uint16_t metricsPort = 6001;
    int c = 0;
    std::string name = "metrics-app";
    std::string id;
    std::string stringVal = "aaa";
    uint32_t instance = getContainerInstance();
    std::vector<std::reference_wrapper<prometheus::Gauge>> m_gauges;

    ::signal(SIGINT, &sig_handler);
    ::signal(SIGTERM, &sig_handler);
    std::srand(static_cast<uint32_t>(std::time(nullptr))); // use current time as seed for random generator

    while ((c = getopt(argc, argv, "f:l:?")) != EOF) {
        switch (c) {
            case 'f':
                configFile = optarg;
                break;
            case 'l':
                logLevel = std::stoi(optarg);
                break;
            case '?':
            default:
                usage();
                return (1);
        }
    }
    auto logger = spdlog::stdout_logger_mt("metrics-app");
    // Log format:
    // 2022-05-07 20:27:55.585|metrics-app|3425239|I|XPUB Port 9200 XSUB Port 9210
    logger->set_pattern("%Y-%m-%d %H:%M:%S.%e|metrics-app|%t|%L|%v");
    // Set the log level for filtering
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));
    std::ifstream ifs(configFile);
    if (ifs.good()) {
        std::stringstream stream;
        stream << ifs.rdbuf();
        try {
            YAML::Node m_yaml = YAML::Load(stream.str());

            if (m_yaml["log-level"]) {
                logLevel = m_yaml["log-level"].as<int>();
            }
            if (m_yaml["metrics-port"]) {
                metricsPort = m_yaml["metrics-port"].as<uint16_t>();
            }
            if (m_yaml["name"]) {
                name = m_yaml["name"].as<std::string>();
            }
            if (m_yaml["id"]) {
                id = m_yaml["id"].as<std::string>();
            }
        } catch (...) {
            logger->error("Error parsing config file");
        }
    }

    name = fmt::format("{}-{}", name, instance);
    // Update logging pattern to reflect the service name
    auto pattern = fmt::format("%Y-%m-%d %H:%M:%S.%e|{}|%t|%L|%v", name);
    logger->set_pattern(pattern);

    // Set the log level for filtering
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));

    std::string metricsUrl = fmt::format("0.0.0.0:{}", metricsPort);

    MetricsCore core(metricsUrl);

    logger->info("{} id {} exposing metrics to {}", name, id, metricsUrl);

    auto &requests = core.buildCounter("requests", "incoming requests");
    auto &requestTotal = requests.Add({{"type", "summary"}});
    auto &requestType1 = requests.Add({{"type", "1"}});
    auto &requestType2 = requests.Add({{"type", "2"}});

    // Build a histogram for latency
    prometheus::Histogram::BucketBoundaries buckets{ 0.05, 5, 10 };

    auto &latency = core.buildHistogram("latency", "message latency in seconds");
    auto &latencyTotal = latency.Add({{"type", "summary"}}, buckets);

    auto &count = core.buildCounter("counter", "message count");
    auto &count1 = count.Add({{"type", "1"}});
    auto &count2 = count.Add({{"type", "2"}});
    auto &count3 = count.Add({{"type", "3"}});
    auto &count4 = count.Add({{"type", "4"}});


    // Build an info metric exposing the service name and id
    //auto &info = 
    core.buildInfo("info", "service info", {{"name", name}, {"id", id}});

    core.start();

    while (running.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Simulate 100 incoming requests
        for (int i = 0; i < 100; i++) {
            const auto random_value = std::rand();
            if (random_value & 1) {
                requestType1.Increment();
                requestTotal.Increment();
            }
            if (random_value & 2) {
                requestType2.Increment();
                requestTotal.Increment();
            }

            // Get random value and treat it as ms
            const auto latency = std::rand() % 10000;
            // Convert to seconds
            double latencyMs = (double) latency / 1000.0;
            latencyTotal.Observe(latencyMs);

            count1.Increment(1);
            count2.Increment(2);
            count3.Increment(3);
            count4.Increment(4);
        }

    }
}