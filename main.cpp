#include <iostream>
#include "timer.h"
#include "test.h"

int sum(int a, int b)
{
    std::cout << "Sum: " << a + b << std::endl;
    return a + b;
}

void print_message(const std::string& msg)
{
    std::cout << msg << std::endl;
}

int main()
{
    // 最多触发10次, 等待2000ms后触发, 回调函数为: 非静态成员函数
// Trigger at most 10 times, wait for 2000ms before triggering, callback function: non-static member function
    Timer timer1(10);
    MyClass my;
    timer1.waitThenStart(2000, 100, &MyClass::operator1, &my, 1, 2);

    // 无限触发, 不等待直接触发, 回调函数为: 静态成员函数
    // Trigger infinitely, trigger immediately without waiting, callback function: static member function
    Timer timer2;
    timer2.start(500, &MyClass::operator2, 1, 2, "static function");

    // 最多触发50次, 不等待直接触发, 回调函数为: 非静态成员函数
    // Trigger at most 50 times, trigger immediately without waiting, callback function: non-static member function
    Timer::Timer_p sp = std::make_shared<Timer>(50);
    sp->start(800, [](std::string msg) {
        std::cout << msg << std::endl;
        }, "shared_ptr lamda function");

    // 单次触发, 等待5000ms后触发, 回调函数为: 静态成员函数
    // Trigger once, wait for 5000ms before triggering, callback function: static member function
    Timer::SingleShot(5000, &MyClass::operator2, 1, 2, "SingleShot-static-function");

    // 单次触发, 等待200ms后触发, 回调函数为: 普通函数
    // Trigger once, wait for 200ms before triggering, callback function: normal function
    Timer::SingleShot(200, print_message, "SingleShot-normal-function");

    // 单次触发, 等待1000ms后触发, 回调函数为: 非静态函数, 阻塞等待完成得到结果(1000ms后得到结果)
    // Trigger once, wait for 1000ms before triggering, callback function: non-static function, block to wait for the result (result will be obtained after 1000ms)
    auto a = Timer::SingleShot(1000, std::bind(&MyClass::operator1, &my, 1, 1));
    std::cout << a.get() << std::endl;


    system("pause");

    return 0;
}
