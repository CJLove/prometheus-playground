#pragma once
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>

class CheckinHistory {
public:
    CheckinHistory();

    CheckinHistory(const CheckinHistory &rhs);

    ~CheckinHistory() = default;

    void configure(std::string name, time_t intervalMs, uint32_t threshold);

    size_t capacity() const;

    size_t size() const; 

    std::string name() const;

    bool empty() const;

    bool full() const;

    void checkin(time_t now);

    bool isOk(time_t now);

private:
    std::string m_name;
    time_t m_intervalMs;
    size_t m_maxSize;
    size_t m_head;
    size_t m_tail;
    bool m_full;
    std::shared_ptr<spdlog::logger> m_logger;
    std::vector<time_t> m_checkins;
    std::mutex m_mutex;
};