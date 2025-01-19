# This is a timer component similar to QTimer that can run in standard C++.
## Platform
It can run on any platform that supports C++ 11 to C++ 20 standards. The test platform is Windows 11, MSVC C++ 11 to C++ 20.

## Features
Using std::async from C++ 11 for asynchronous tasks, this timer component implements asynchronous timing tasks, single-trigger timing tasks, and more... Using C++ variadic templates, it provides a timer interface that can forward any function as a callback.

## Interface Description
The example demonstrates how to call static class functions, non-static member functions, regular functions, etc. Note that when calling non-static class member functions, you need to use std::bind or lambda functions for wrapping.


```C++
// Trigger infinitely, trigger immediately without waiting, callback function: static member function
Timer timer2;
timer2.start(500, &MyClass::operator2, 1, 2, "static function");

// lamda
Timer::Timer_p sp = std::make_shared<Timer>(50);
sp->start(800, [](std::string msg) {
    std::cout << msg << std::endl;
    }, "shared_ptr lamda function");
```
```C++
template<typename Func, typename... Args>
void waitThenStart(int64_t waitTime, int64_t interval, Func&& callback, Args&&... args);
```
```C++
// Trigger at most 10 times, wait for 2000ms before triggering, callback function: non-static member function
Timer timer1(10);
MyClass my;
timer1.waitThenStart(2000, 100, &MyClass::operator1, &my, 1, 2);
```
```C++
template<typename Func, typename... Args>
static auto SingleShot(int64_t waitTime, Func&& callback, Args&&... args);
```
```C++
// Trigger once, wait for 5000ms before triggering, callback function: static member function
Timer::SingleShot(5000, &MyClass::operator2, 1, 2, "SingleShot-static-function");

// Trigger once, wait for 1000ms before triggering, callback function: non-static function, block to wait for the result (result will be obtained after 1000ms)
auto a = Timer::SingleShot(1000, std::bind(&MyClass::operator1, &my, 1, 1));
std::cout << a.get() << std::endl;
```
### For more details, please see the test case

# 这是一个类似于QTimer的定时器组件, 可以在标准c++中运行

## 平台
可在任意支持C++ 11 ~ 20标准中运行, 但是测试平台为Windows 11 , MSVC C++ 11 ~ C++ 20

## 功能特性
使用C++ 11中的std::async异步任务函数, 实现异步定时任务, 计时任务(单次触发)等等...
使用C++中的可变参数模板, 构造了一个可以传导任意函数的定时器接口或者计时器接口

## 接口介绍
其中示例中介绍了调用类内静态函数, 非静态函数, 普通函数等等, 需要注意的是调用类内非静态函数的时候需要使用std::bind或者lamda函数进行包装 
```C++
template<typename Func, typename... Args>
void start(int64_t interval, Func&& callback, Args&&... args);
```

```C++
// 无限触发, 不等待直接触发, 回调函数为: 静态成员函数
Timer timer2;
timer2.start(500, &MyClass::operator2, 1, 2, "static function");

// lamda
Timer::Timer_p sp = std::make_shared<Timer>(50);
sp->start(800, [](std::string msg) {
    std::cout << msg << std::endl;
    }, "shared_ptr lamda function");
```
```C++
template<typename Func, typename... Args>
void waitThenStart(int64_t waitTime, int64_t interval, Func&& callback, Args&&... args);
```
```C++
// 最多触发10次, 等待2000ms后触发, 回调函数为: 非静态成员函数
Timer timer1(10);
MyClass my;
timer1.waitThenStart(2000, 100, &MyClass::operator1, &my, 1, 2);
```
```C++
template<typename Func, typename... Args>
static auto SingleShot(int64_t waitTime, Func&& callback, Args&&... args);
```
```C++
// 单次触发, 等待5000ms后触发, 回调函数为: 静态成员函数
Timer::SingleShot(5000, &MyClass::operator2, 1, 2, "SingleShot-static-function");

// Trigger once, wait for 1000ms before triggering, callback function: non-static function, block to wait for the result (result will be obtained after 1000ms)
auto a = Timer::SingleShot(1000, std::bind(&MyClass::operator1, &my, 1, 1));
std::cout << a.get() << std::endl;
```

### 更详细的请看测试用例
