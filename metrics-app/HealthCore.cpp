#include "HealthCore.h"
#include "CheckinHistory.h"
#include "prometheus/CivetServer.h"
#include <spdlog/spdlog.h>
#include <vector>
#include <sys/time.h>
#include <ctime>




struct HealthCore::st_impl: public CivetHandler {
    CivetServer m_server;
    std::shared_ptr<spdlog::logger> m_logger;
    std::vector<CheckinHistory> m_threads;
    bool m_override;

    time_t timeMs() {
        struct timeval timeNow{};
        gettimeofday(&timeNow, nullptr);
        time_t msecTime = (timeNow.tv_sec * 1000) + (timeNow.tv_usec / 1000);
        return msecTime;
    }

    bool handleGet(CivetServer *server, struct mg_connection *conn);

    void checkin(uint32_t thread);

    st_impl(const std::string &uri, const std::vector<ThreadConfig> &threadConfig):
        m_server({"listening_ports", uri, "num_threads", "2"}),
        m_logger(spdlog::get("metrics-app")),
        m_override(false)
    {
        // Add handler for /healthz endpoint
        m_server.addHandler("/healthz",*this);

        m_threads.resize(threadConfig.size());
        for (size_t i = 0; i < threadConfig.size(); i++) {
            m_threads[i].configure(threadConfig[i].m_name, threadConfig[i].m_intervalMs, threadConfig[i].m_checkinThreshold);
        }
    }

};

bool HealthCore::st_impl::handleGet(CivetServer *, struct mg_connection *conn)
{
    m_logger->info("GET /healthz");
    if (m_override) {
        mg_printf(conn,"HTTP/1.1 400 Override failure\r\n");       
        return true;
    } else {
        time_t now = timeMs();
        for (auto &thread: m_threads) {
            if (!thread.isOk(now)) {
                m_logger->debug("Thread {} unhealthy at {}", thread.name(), now);
                mg_printf(conn, "HTTP/1.1 400 Thread %s failure\r\n", thread.name().c_str());
                return true;
            }
            m_logger->debug("Thread {} OK at {}", thread.name(), now);
        }
    }
    mg_printf(conn,"HTTP/1.1 200 OK\r\n");
    return true;
}

void HealthCore::st_impl::checkin(uint32_t thread)
{
    if (thread < m_threads.size()) {
        m_threads[thread].checkin(timeMs());
    }
}

HealthCore::HealthCore(const std::string &uri, const std::vector<ThreadConfig> &threadConfig):
    m_pImpl(std::make_unique<HealthCore::st_impl>(uri, threadConfig))
{

}

HealthCore::~HealthCore()
{

}

void HealthCore::checkin(uint32_t thread)
{
    m_pImpl->checkin(thread);
}

void HealthCore::force(bool value)
{
    m_pImpl->m_override = value;
}