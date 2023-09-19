#include "MetricsCore.h"
#include <prometheus/counter.h>
#include <prometheus/gauge.h>
#include <prometheus/histogram.h>
#include <prometheus/summary.h>

MetricsCore::MetricsCore(const std::string &url):
    m_exposer(url),
    m_registry(std::make_shared<prometheus::Registry>())
{

}

MetricsCore::~MetricsCore()
{

}

std::shared_ptr<prometheus::Registry>
MetricsCore::registry()
{
    return m_registry;
}

void
MetricsCore::start()
{
    m_exposer.RegisterCollectable(m_registry);
}

prometheus::Family<prometheus::Gauge>& 
MetricsCore::buildGauge(const std::string &name, const std::string &helpStr)
{
    return prometheus::BuildGauge().Name(name).Help(helpStr).Register(*m_registry);
}

prometheus::Family<prometheus::Counter>& 
MetricsCore::buildCounter(const std::string &name, const std::string &helpStr)
{
    return prometheus::BuildCounter().Name(name).Help(helpStr).Register(*m_registry);
}

prometheus::Family<prometheus::Histogram>& 
MetricsCore::buildHistogram(const std::string &name, const std::string &helpStr)
{
    return prometheus::BuildHistogram().Name(name).Help(helpStr).Register(*m_registry);
}

prometheus::Family<prometheus::Summary>& 
MetricsCore::buildSummary(const std::string &name, const std::string &helpStr)
{
    return prometheus::BuildSummary().Name(name).Help(helpStr).Register(*m_registry);
}

prometheus::Family<prometheus::Gauge>& 
MetricsCore::buildInfo(const std::string &name, const std::string &helpStr, const prometheus::Labels &labels)
{
    auto &infoGaugeFamily = prometheus::BuildGauge().Name(name).Help(helpStr).Register(*m_registry);
    auto &infoGauge = infoGaugeFamily.Add(labels);
    infoGauge.Set(1);

    return infoGaugeFamily;
}
