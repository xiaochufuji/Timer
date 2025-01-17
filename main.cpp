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
    // 1. print pause
    // 2. print start (500ms/times)
    // 3. print SingleShot (delay 1000ms and print once)
    // 4. print waitThenStart (delay 2000ms and print (100ms/times))
    Timer timer1;
    timer1.waitThenStart(2000, 100, &MyClass::operator2, 1, 2, "waitThenStart");
    Timer::SingleShot(998, &MyClass::operator2, 1, 2, "SingleShot");
    Timer timer2;
    timer2.start(500, &MyClass::operator2, 1, 2, "start");
    Timer::SingleShot(1000, &MyClass::operator2, 1, 2, "the second time shot");
    system("pause");
    return 0;
}
