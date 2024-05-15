#pragma once
#include <memory>
#include <string>
#include <vector>

struct ThreadConfig {
    std::string m_name;
    time_t m_intervalMs;
    uint32_t m_checkinThreshold;
};

/**
 * @brief HealthCore class is responsible for aggregating status of application threads, in support of
 *        supporting an HTTP liveness probe reporting any deadlocked application threads
 */
class HealthCore {
public:
    /**
     * @brief Construct a new HealthCore object
     * 
     * @param uri - listen address and port number
     * @param threadConfig - configuration of application thread(s) which will be checking in
     */
    HealthCore(const std::string &uri, const std::vector<ThreadConfig> &threadConfig);

    /**
     * @brief Destroy the HealthCore object
     */
    ~HealthCore();

    /**
     * @brief Check in for a particular application thread
     * 
     * @param thread - Index of thread checking in(0..x)
     */
    void checkin(uint32_t thread);

    /**
     * @brief Set or clear an override flag which would bypass application thread health checks and return
     *        a liveness probe failure for some other reason
     * 
     * @param value - override flag value
     */
    void force(bool value);

private:
    /**
     * @brief Private implementation
     */
    struct st_impl;
    std::unique_ptr<st_impl> m_pImpl;

};