#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
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
    Timer timer;
    timer.start(100, [] {
        std::cout << "=========" << std::endl;
    });

    Timer timer1;
    MyClass my;
    timer.start(1000, &MyClass::operator1, &my, 1, 2);

    Timer::SingleShot(1000, &MyClass::operator2, 1,2,"123");

    system("pause");
    return 0;
}
