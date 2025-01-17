#ifndef __TIMER_H
#define __TIMER_H
/*
*   use at least C++11
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
#include <limits>

#define INFINITE_LOOPS LLONG_MAX
//#undef __cplusplus
//#define __cplusplus (202002L)

class Timer: public std::enable_shared_from_this<Timer>
{
public:
    using Timer_p = std::shared_ptr<Timer>;
    Timer() = default;
    Timer(int64_t maxLoopTimes);
    ~Timer() { stop(); }

public:
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
    static auto SingleShot(const std::chrono::milliseconds& waitTime, Func&& callback, Args&&... args);

    // setter/getter
    bool running();
    bool haveIntoCallback();    // if used to get into the callback function
    std::chrono::milliseconds interval();
    std::chrono::milliseconds waitTime();
    int64_t loopTimes();
    int64_t maxLoopTimes();
    void setHaveIntoCallback(bool b_haveIntoCallback);
    void setMaxLoopTimes(int64_t i_maxLoopTimes);


private:
    void _start(bool waitForCompletion = false);
    void _stop();
    void _singleShot(bool waitForCompletion = true);
    void _join();

private:

    std::atomic<bool> m_running = false;
    std::atomic<bool> m_haveIntoCallback = false;   // 进入过回调, 则为true
    std::chrono::milliseconds m_interval{ 100 };
    std::chrono::milliseconds m_waitTime{ 100 };
    std::function<void()> m_callback;
    int64_t m_loopTimes = 0;
    int64_t m_maxLoopTimes = 300;
    std::future<void> m_task;
};

inline Timer::Timer(int64_t maxLoopTimes)
{
    m_maxLoopTimes = maxLoopTimes;
}

inline void Timer::reset()
{
    stop();
    m_interval = std::chrono::milliseconds{ 500 };
    m_waitTime = std::chrono::milliseconds{ 500 };
}

template<typename Func, typename... Args>
void Timer::start(int64_t interval, Func&& callback, Args&&... args)
{
    if (m_callback)  return;
    m_interval = std::chrono::milliseconds{ interval };
#if __cplusplus >= 202002L 
    m_callback = std::function<void()>(
        [cb = std::forward<Func>(callback), ... args = std::forward<Args>(args)]() {
            cb(args...);
        });
    _start();
#else
    m_callback = std::function<void()>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() {
            cb();
        });
    _start();
#endif
}

template<typename Func, typename... Args>
void Timer::start(const std::chrono::milliseconds& interval, Func&& callback, Args&&... args)
{
    if (m_callback)  return;
    m_interval = interval;
#if __cplusplus >= 202002L 
    m_callback = std::function<void()>(
        [cb = std::forward<Func>(callback), ... args = std::forward<Args>(args)]() {
            cb(args...);
        });
    _start();
#else
    m_callback = std::function<void()>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() {
            cb();
        });
    _start();
#endif
}

template<typename Func, typename ...Args>
void Timer::waitThenStart(int64_t waiteTime, int64_t interval, Func&& callback, Args && ...args)
{
    if (m_callback)  return;
    m_waitTime = std::chrono::milliseconds{ waiteTime };
    m_interval = std::chrono::milliseconds{ interval };
#if __cplusplus >= 202002L 
    m_callback = std::function<void()>(
        [cb = std::forward<Func>(callback), ... args = std::forward<Args>(args)]() {
            cb(args...);
        });
    _start(true);
#else
    m_callback = std::function<void()>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() {
            cb();
        });
    _start(true);
#endif
}

template<typename Func, typename ...Args>
void Timer::waitThenStart(const std::chrono::milliseconds& waiteTime, const std::chrono::milliseconds& interval, Func&& callback, Args && ...args)
{
    if (m_callback)  return;
    m_waitTime = std::chrono::milliseconds{ waiteTime };
    m_interval = std::chrono::milliseconds{ interval };
#if __cplusplus >= 202002L 
    m_callback = std::function<void()>(
        [cb = std::forward<Func>(callback), ... args = std::forward<Args>(args)]() {
            cb(args...);
        });
    _start(true);
#else
    m_callback = std::function<void()>(
        [cb = std::bind(std::forward<Func>(callback), std::forward<Args>(args)...)]() {
            cb();
        });
    _start(true);
#endif
}

template<typename Func, typename ...Args>
auto Timer::SingleShot(int64_t waitTime, Func&& callback, Args && ...args)
{
    Timer_p timer = std::make_shared<Timer>();
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
    Timer_p timer = std::make_shared<Timer>();
    using RetType = decltype(callback(args...));
    if (timer->m_callback)  return std::future<RetType>{};
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
    timer->_singleShot(true);
    return _future;
}

void Timer::stop()
{
    _stop();
}

inline bool Timer::running()
{
    return m_running;
}

inline bool Timer::haveIntoCallback()
{
    return m_haveIntoCallback;
}

std::chrono::milliseconds Timer::interval()
{
    return m_interval;
}

inline std::chrono::milliseconds Timer::waitTime()
{
    return m_waitTime;
}

inline int64_t Timer::loopTimes()
{
    return m_loopTimes;
}

inline int64_t Timer::maxLoopTimes()
{
    return m_maxLoopTimes;
}

inline void Timer::setHaveIntoCallback(bool b_haveIntoCallback)
{
    m_haveIntoCallback = b_haveIntoCallback;
}

inline void Timer::setMaxLoopTimes(int64_t i_maxLoopTimes)
{
    m_maxLoopTimes = i_maxLoopTimes;
}

void Timer::_start(bool waitForCompletion)
{
    m_running = true;
    m_task = std::async(std::launch::async, [this, waitForCompletion]() {
        if (waitForCompletion)
        {
            std::this_thread::sleep_for(m_waitTime);
        }
        while (m_running)
        {
            if (m_loopTimes++ > m_maxLoopTimes && m_maxLoopTimes != INFINITE_LOOPS) break;
            try
            {
                std::this_thread::sleep_for(m_interval);
                if (m_callback) {
                    m_callback();
                    m_haveIntoCallback = true;
                }
            }
            catch (const std::exception& e) {
                std::string error = std::string("async _start: Caught exception: ") + std::string(e.what());
                std::cout << error << std::endl;
                m_running = false;
            }
            catch (...) {
                std::string error = std::string("async _start: Caught unknown exception!");
                std::cout << error << std::endl;
                m_running = false;
            }
        }
    });
}

inline void Timer::_stop()
{
    m_loopTimes = 0;
    m_running = false;
    m_callback = nullptr;
    m_haveIntoCallback = false;
    _join();
}

void Timer::_singleShot(bool waitForCompletion)
{
    Timer_p timer = shared_from_this();
    timer->m_task = std::async(std::launch::async, [timer = timer, waitForCompletion]() {
    try
    {
        if (waitForCompletion)
        {
            std::this_thread::sleep_for(timer->m_waitTime);
        }
        if (timer->m_callback) {
            timer->m_callback();
        }
    }
    catch (const std::exception& e) {
        std::string error = std::string("async _singleShot: Caught exception: ") + std::string(e.what());
        std::cout << error << std::endl;
    }
    catch (...) {
        std::string error = std::string("async _singleShot: Caught unknown exception!");
        std::cout << error << std::endl;
    }
    });
}

inline void Timer::_join()
{
    if (m_task.valid()) {
        try {
            std::future_status status = m_task.wait_for(std::chrono::milliseconds(100));  // 设置超时
            if (status == std::future_status::timeout) {
                std::cout << "Async task took too long to complete." << std::endl;
            }
            else {
                m_task.get();
            }
        }
        catch (const std::future_error& e) {
            std::cout << "Error while waiting for future: " << e.what() << std::endl;
        }
    }
}

#endif  // HEADER