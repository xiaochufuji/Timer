#pragma once
class MyClass
{
public:
    MyClass();
    ~MyClass();
    int operator1(int a, int b);
    static inline std::string operator2(int a, int b, const std::string& str);
private:

};

MyClass::MyClass()
{
}

MyClass::~MyClass()
{
}
int MyClass::operator1(int a, int b)
{
    int c = a + b;
    std::cout << "hello world1" << std::endl;
    return c;
}

inline std::string MyClass::operator2(int a, int b, const std::string& str)
{
    std::cout << str << std::endl;
    return "helloworld";
}
