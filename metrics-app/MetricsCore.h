#pragma once
#include <memory>
#include <prometheus/exposer.h>
#include <prometheus/family.h>
#include <prometheus/registry.h>

#include <string>

// Forward declarations
namespace prometheus {
class Gauge;
class Counter;
class Histogram;
class Summary;
};  // namespace prometheus

/**
 * @brief MetricsCore manages the lifecycle for Prometheus-cpp Exposer and Registry instances
 *        and provides factory methods for creating Prometheus metric families:
 *          1. Gauge
 *          2. Counter
 *          3. Histogram
 *          4. Summary
 *          5. Info (realized as a Gauge with specified set of labels and fixed value of 1)
 */
class MetricsCore {
public:
    /**
     * @brief Constructor
     * 
     * @param url - IP address/port number to expose metrics
     */
    MetricsCore(const std::string &url);

    /**
     * @brief Destructor
     */
    ~MetricsCore();

    void start();

    /**
     * @brief Accessor to access the Registry instance
     * 
     * @return std::shared_ptr<prometheus::Registry> 
     */
    std::shared_ptr<prometheus::Registry> registry();

    /**
     * @brief Create a Gauge metric with the specified name and help string
     * 
     * @param name - metric name
     * @param helpStr - metric help string
     * @return prometheus::Family<prometheus::Gauge>& - reference to the Gauge metric
     */
    prometheus::Family<prometheus::Gauge> &buildGauge(const std::string &name, const std::string &helpStr);

    /**
     * @brief Create a Counter metric with the specified name and help string
     * 
     * @param name - metric name
     * @param helpStr - metric help string
     * @return prometheus::Family<prometheus::Counter>& - reference to the Counter metric 
     */
    prometheus::Family<prometheus::Counter> &buildCounter(const std::string &name, const std::string &helpStr);

    /**
     * @brief Create a Histogram metric with the specified name and help string
     * 
     * @param name - metric name
     * @param helpStr - metric help string
     * @return prometheus::Family<prometheus::Histogram>& - reference to the Histogram metric 
     */
    prometheus::Family<prometheus::Histogram> &buildHistogram(const std::string &name, const std::string &helpStr);

    /**
     * @brief Create a Summary metric with the specified name and help string
     * 
     * @param name - metric name
     * @param helpStr - metric help string
     * @return prometheus::Family<prometheus::Summary>& - reference to the Summary metric
     */
    prometheus::Family<prometheus::Summary> &buildSummary(const std::string &name, const std::string &helpStr);

    /**
     * @brief Create an Info metric with the specified name, help string and labels/values
     * 
     * @param name - metric name
     * @param helpStr - metric help string
     * @param labels - labels/values
     * @return prometheus::Family<prometheus::Gauge>& - reference to the Info metric
     */
    prometheus::Family<prometheus::Gauge> &buildInfo(const std::string &name, const std::string &helpStr,
                                                     const prometheus::Labels &labels);

private:
    /**
     * @brief Prometheus exposer for HTTP metric scraping
     */
    prometheus::Exposer m_exposer;

    /**
     * @brief Prometheus registry for creating and managing metrics
     */
    std::shared_ptr<prometheus::Registry> m_registry;
};