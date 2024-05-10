#include <spdlog/spdlog.h>
#include <memory>
#include <vector>
#include "CheckinHistory.h"

CheckinHistory::CheckinHistory():
        m_intervalMs(0),
        m_maxSize(0),
        m_head(0),
        m_tail(0),
        m_full(false),
        m_logger(spdlog::get("metrics-app"))
    {
    }

CheckinHistory::CheckinHistory(const CheckinHistory &rhs):
        m_name(rhs.m_name),
        m_intervalMs(rhs.m_intervalMs),
        m_maxSize(rhs.m_maxSize),
        m_head(rhs.m_head),
        m_tail(rhs.m_tail),
        m_full(rhs.m_full),
        m_logger(rhs.m_logger),
        m_checkins(rhs.m_checkins)
    {}


    void CheckinHistory::configure(std::string name, time_t intervalMs, uint32_t threshold)
    {
        m_name = name;
        m_maxSize = threshold;
        m_intervalMs = intervalMs;
        m_checkins.resize(threshold);
    }

    size_t CheckinHistory::capacity() const { return m_maxSize; }

    size_t CheckinHistory::size() const {
        size_t size = m_maxSize;
        if (!m_full) {
            if (m_head >= m_tail) {
                size = m_head - m_tail;
            } else {
                size = m_maxSize + m_head - m_tail;
            }
        }
        return size;
    }

    std::string CheckinHistory::name() const { return m_name; }

    bool CheckinHistory::empty() const { return (!m_full && (m_head == m_tail)); }

    bool CheckinHistory::full() const { return m_full; }

    void CheckinHistory::checkin(time_t now)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_logger->debug("Thread {} check-in at  {}", m_name, now);
        m_checkins[m_head] = now;
        if (m_full) {
            m_tail = (m_tail + 1) % m_maxSize;
        }
        m_head = (m_head+1) % m_maxSize;
        m_full = (m_head == m_tail);
    }

    bool CheckinHistory::isOk(time_t now)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        time_t start = now - m_intervalMs;
        if (m_full) {
            return (m_checkins[m_tail] >= start);
        }
        return false;
    }