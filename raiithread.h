#pragma once
#include <iostream>
#include <thread>
#include <future>
#include <functional>
#include <exception>

class RAIIThread {
public:
    template <typename Func, typename... Args>
    explicit RAIIThread(Func&& f, Args&&... args)
    {
        // 启动线程并将任务传递给线程
        m_thread = std::thread([callback = std::bind(std::forward<Func>(f), std::forward<Args>(args)...)]() mutable {
            callback();
            });
    }

    ~RAIIThread()
    {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    RAIIThread(const RAIIThread&) = delete;
    RAIIThread& operator=(const RAIIThread&) = delete;

    RAIIThread(RAIIThread&& other) noexcept : m_thread(std::move(other.m_thread)) {}
    RAIIThread& operator=(RAIIThread&& other) noexcept {
        if (this != &other) {
            if (m_thread.joinable()) {
                m_thread.join();
            }
            m_thread = std::move(other.m_thread);
        }
        return *this;
    }

private:
    std::thread m_thread;
};


