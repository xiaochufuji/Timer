//#include "timer.h"
//#include <thread>
//
//using std::async;
//using std::future;
//using std::function;
//
//
//Timer* Timer::g_instance = new Timer;
//
//
//Timer::Timer()
//{
//	reset();
//}
//
//
//Timer::~Timer()
//{
//	m_running = false;
//}
//
//
//Timer* Timer::GetInstance()
//{
//	return g_instance;
//}
//
//
//void Timer::reset()
//{
//	g_stop = std::chrono::steady_clock::now();
//	g_start = std::chrono::steady_clock::now();
//}
//
//
//std::chrono::milliseconds Timer::getDuration() const
//{
//	return std::chrono::duration_cast<std::chrono::milliseconds>(g_stop - g_start);
//}
//
//
//int64_t Timer::getDurationCount() const
//{
//	return std::chrono::duration_cast<std::chrono::milliseconds>(g_stop - g_start).count();
//}
//
//
//void Timer::setRunning(bool running)
//{
//	m_running = running;
//}
//
//
//void Timer::setInterval(std::chrono::milliseconds interval)
//{
//	m_interval = interval;
//}
//
//
//void Timer::setCallback(Func callback)
//{
//	m_callback = callback;
//}
//
//
//void Timer::start()
//{
//	if (!m_callback) return;
//	m_task = async(std::launch::async, [this]() {
//		m_running = true;
//		while (m_running)
//		{
//			// block for some time and work for one time
//			std::this_thread::sleep_for(m_interval);
//			m_callback();
//		}
//		});
//}
//
//
//void Timer::start(Func callback, int64_t interval)
//{
//	m_callback = callback;
//	m_interval = std::chrono::milliseconds{ interval };
//	start();
//}
//
//
//void Timer::start(Func callback, std::chrono::milliseconds interval)
//{
//
//	m_callback = callback;
//	m_interval = interval;
//	start();
//}
//
//
//void Timer::stop()
//{
//	m_running = false;
//	if (m_task.valid()) {
//		m_task.get();  // wait for stop
//	}
//}
//
//
//void Timer::_timerStart()
//{
//	g_start = std::chrono::steady_clock::now();
//}
//
//
//void Timer::_timerStop()
//{
//	g_stop = std::chrono::steady_clock::now();
//}