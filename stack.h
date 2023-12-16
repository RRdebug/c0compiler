/*
    自定义一个栈结构，底层还是用STL
    主要是为了POP操作的方便
    重写后的栈POP时返回栈顶元素
*/


#ifndef STACK
#define STACK

#include <iostream>
#include <stack>

//栈，使用模板泛化
template<typename T>
class MyStack {
private:
    std::stack<T> s;

public:
    void push(const T& value) 
    {
        s.push(value);
    }

    T pop()         //主要修改了pop操作
    {
        if (s.empty()) 
        {
            throw runtime_error("栈为空");
        }
        T topValue = s.top();
        s.pop();
        return topValue;
    }

    T top() const   
    {
        if (s.empty()) 
        {
            throw runtime_error("栈为空");
        }
        return s.top();
    }

    bool empty() const 
    {
        return s.empty();
    }
};

#endif