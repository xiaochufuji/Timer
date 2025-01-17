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
    Timer timer1(10);
    MyClass my;
    timer1.waitThenStart(2000, 100, &MyClass::operator1, &my , 1, 2);

    // 无限触发, 不等待直接触发, 回调函数为: 静态成员函数
    Timer timer2;
    timer2.start(500, &MyClass::operator2, 1, 2, "static function");

    // 最多触发50次, 不等待直接触发, 回调函数为: 非静态成员函数
    Timer::Timer_p sp = std::make_shared<Timer>(50);
    sp->start(800, [](std::string msg) {
        std::cout << msg << std::endl;
    }, "shared_ptr lamda function");

    // 单词触发, 等待5000ms后触发, 回调函数为: 静态成员函数
    Timer::SingleShot(5000, &MyClass::operator2, 1, 2, "SingleShot-static-function");

    // 单次触发, 等待200ms后触发, 回调函数为: 普通函数
    Timer::SingleShot(200, print_message, "SingleShot-normal-function");
    
    // 单次触发, 等待1000ms后触发, 回调函数为: 非静态函数, 阻塞等待完成得到结果(1000ms后得到结果)
    auto a = Timer::SingleShot(1000, std::bind(&MyClass::operator1, &my, 1, 1));
    std::cout << a.get() << std::endl;

    system("pause");

    return 0;
}
