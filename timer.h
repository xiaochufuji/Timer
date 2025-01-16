#ifndef __TIMER_H
#define __TIMER_H
/*
*   use at least C++17
*   it's similar with QTimer in Qt platform
*   
*/
#include <chrono>
#include <future>
#include <functional>
#include <atomic>
#include <thread>
#include <iostream>
#include <memory>

#define ASYNC_OR_THREAD

class Timer
{
public:
    using Timer_p = std::shared_ptr<Timer>;
    Timer() = default;
    ~Timer() { stop(); }

public:
    // singleton mode
    static Timer* GetGlobalInstance();
    void reset();

    // start the interval and run now
    template<typename Func, typename... Args>
    void start(int64_t interval, Func&& callback, Args&&... args);
    template<typename Func, typename... Args>
    void start(const std::chrono::milliseconds& interval, Func&& callback, Args&&... args);
    // start the interval after wait for time
    template<typename Func, typename... Args>
    void waitThenStart(int64_t waitTime, int64_t interval, Func&& callback, Args&&... args);
    template<typename Func, typename... Args>
    void waitThenStart(const std::chrono::milliseconds& waitTime, const std::chrono::milliseconds& interval, Func&& callback, Args&&... args);
    void stop();

    // single shot 
    template<typename Func, typename... Args>
    static auto SingleShot(int64_t waitTime, Func&& callback, Args&&... args);
    template<typename Func, typename... Args>
    auto SingleShot(const std::chrono::milliseconds& waitTime, Func&& callback, Args&&... args);

private:
    void _start(bool wait = false);
    void _stop();
    void _singleShot(bool wait = true);

private:
    bool m_running = false;
    std::chrono::milliseconds m_interval{ 500 };
    std::chrono::milliseconds m_waitTime{ 500 };
    std::function<void()> m_callback;
#ifdef ASYNC_OR_THREAD
    std::future<void> m_task;
#else
    std::thread m_thread;  
#endif
};

// global instance
static Timer g_instance; 
Timer* Timer::GetGlobalInstance()
{
    return &g_instance;
}

inline void Timer::reset()
{
    m_running = false;
    m_callback = nullptr;
    m_interval = std::chrono::milliseconds{ 500 };
    m_waitTime = std::chrono::milliseconds{ 500 };
    stop();
}

template<typename Func, typename... Args>
void Timer::start(int64_t interval, Func&& callback, Args&&... args)
{
    if (m_callback)  return;
    m_interval = std::chrono::milliseconds{ interval };
    m_callback = std::function<void()>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() {
            cb();
        });
    _start();
}

template<typename Func, typename... Args>
void Timer::start(const std::chrono::milliseconds& interval, Func&& callback, Args&&... args)
{
    if (m_callback)  return;
    m_interval = interval;
    m_callback = std::function<void()>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() {
            cb();
        });
    _start();
    //using RetType = decltype(callback(args...));
    //auto _task = std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<Func>(callback), std::forward<Args>(args)...));
    //std::future<RetType> _future = _task->get_future();
    //m_callback = [_task] {
    //    _task();
    //    };
    //_start();
    //return _future;
}

template<typename Func, typename ...Args>
 void Timer::waitThenStart(int64_t waiteTime, int64_t interval, Func&& callback, Args && ...args)
{
    if (m_callback)  return;
    m_waitTime = std::chrono::milliseconds{ waiteTime };
    m_interval = std::chrono::milliseconds{ interval };
    m_callback = std::function<void()>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() {
            cb();
        });
    _start(true);
}

template<typename Func, typename ...Args>
 void Timer::waitThenStart(const std::chrono::milliseconds& waiteTime, const std::chrono::milliseconds& interval, Func&& callback, Args && ...args)
{
    if (m_callback)  return;
    m_waitTime = std::chrono::milliseconds{ waiteTime };
    m_interval = std::chrono::milliseconds{ interval };
    m_callback = std::function<void()>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() {
            cb();
        });
    _start(true);
}

template<typename Func, typename ...Args>
auto Timer::SingleShot(int64_t waitTime, Func&& callback, Args && ...args)
{
    Timer* timer = GetGlobalInstance();
    using RetType = decltype(callback(args...));
    if (timer->m_callback)  return std::future<RetType>{};
    timer->m_waitTime = std::chrono::milliseconds{ waitTime };
#if __cplusplus >= 202002L 
    auto _task = std::make_shared<std::packaged_task<RetType()>>(
        [callback = std::forward<Func>(callback), ...args = std::forward<Args>(args)]() mutable {
            return callback(args...);
        });
#else
    auto _task = std::make_shared<std::packaged_task<RetType()>>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() mutable {
            return cb();
        });
#endif
    std::future<RetType> _future = _task->get_future();
    timer->m_callback = [_task] {
        (*_task)();
    };
    timer->_singleShot(true);
    return _future;
}

template<typename Func, typename ...Args>
auto Timer::SingleShot(const std::chrono::milliseconds& waitTime, Func&& callback, Args && ...args)
{
    Timer* timer = GetGlobalInstance();
    using RetType = decltype(callback(args...));
    if (timer->m_callback)  return std::future<RetType>{};
    using RetType = decltype(callback(args...));
    timer->m_waitTime = waitTime;
#if __cplusplus >= 202002L 
    auto _task = std::make_shared<std::packaged_task<RetType()>>(
        [callback = std::forward<Func>(callback), ...args = std::forward<Args>(args)]() mutable {
            return callback(args...);
        });
#else
    auto _task = std::make_shared<std::packaged_task<RetType()>>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() mutable {
            return cb();
        });
#endif
    std::future<RetType> _future = _task->get_future();
    timer->m_callback = [_task] {
        (*_task)();
    };
    timer->_singleShot(timer, true);
    return _future;
}

void Timer::stop()
{
    _stop();
}

void Timer::_start(bool wait)
{
    m_running = true;
    if (wait)
    {
        std::this_thread::sleep_for(m_waitTime);
    }
#ifdef ASYNC_OR_THREAD
    m_task = std::async(std::launch::async, [this]() {
        while (m_running)
        {
            try
            {
                std::this_thread::sleep_for(m_interval);
                if (m_callback) {
                    m_callback();
                }
            }
            catch (const std::exception& e) {
                std::string error = std::string("Caught exception: ") + std::string(e.what());
                std::cout << error << std::endl;
                m_running = false;
            }
            catch (...) {
                std::string error = std::string("Caught unknown exception!");
                std::cout << error << std::endl;
                m_running = false;
            }
        }
    });
#else
    m_thread = std::thread([this, wait]() {
        m_running = true;
        while (m_running) {
            try {
                std::this_thread::sleep_for(m_interval);
                if (m_callback) {
                    m_callback();
                }
            }
            catch (const std::exception& e) {
                std::string error = std::string("Caught exception: ") + std::string(e.what());
                std::cout << error << std::endl;
                m_running = false;
            }
            catch (...) {
                std::string error = std::string("Caught unknown exception!");
                std::cout << error << std::endl;
                m_running = false;
            }
        }
        });
#endif
    if (!m_running)
    {
        this->stop();
    }
}

inline void Timer::_stop()
{
    m_running = false;
    m_callback = nullptr;
#ifdef ASYNC_OR_THREAD
    if (m_task.valid()) {
        m_task.get();
    }
#else
    if (m_thread.joinable()) {
        m_thread.join();
    }
#endif
}

void Timer::_singleShot(bool wait)
{
#ifdef ASYNC_OR_THREAD
    m_task = std::async(std::launch::async, [this, wait]() {
        try
        {
            if (wait)
            {
                std::this_thread::sleep_for(m_waitTime);
            }
            if (m_callback) {
                m_callback();
            }
        }
        catch (const std::exception& e) {
            std::string error = std::string("Caught exception: ") + std::string(e.what());
            std::cout << error << std::endl;
        }
        catch (...) {
            std::string error = std::string("Caught unknown exception!");
            std::cout << error << std::endl;
        }
    });
#else
    m_thread = std::thread([this, wait]() {
        try {
            if (wait)
            {
                std::this_thread::sleep_for(m_waitTime);
            }
            if (m_callback) {
                m_callback();
            }
        }
        catch (const std::exception& e) {
            std::string error = std::string("Caught exception: ") + std::string(e.what());
            std::cout << error << std::endl;
        }
        catch (...) {
            std::string error = std::string("Caught unknown exception!");
            std::cout << error << std::endl;
        }
    });
#endif
}

#endif  // HEADER
