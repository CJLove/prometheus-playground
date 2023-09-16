#pragma once
#include <memory>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <prometheus/family.h>

#include <string>

namespace prometheus {
    class Gauge;
    class Counter;
    class Histogram;
    class Summary;
};

class MetricsCore {
public:
    MetricsCore(const std::string &url);

    ~MetricsCore();

    void start();

    std::shared_ptr<prometheus::Registry> registry();

    prometheus::Family<prometheus::Gauge>& buildGauge(const std::string &name, const std::string &helpStr);

    prometheus::Family<prometheus::Counter>& buildCounter(const std::string &name, const std::string &helpStr);

    prometheus::Family<prometheus::Histogram>& buildHistogram(const std::string &name, const std::string &helpStr);

    prometheus::Family<prometheus::Summary>& buildSummary(const std::string &name, const std::string &helpStr);

private:
    prometheus::Exposer m_exposer;
    std::shared_ptr<prometheus::Registry> m_registry;
};