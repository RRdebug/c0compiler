/*
    �Զ���һ��ջ�ṹ���ײ㻹����STL
    ��Ҫ��Ϊ��POP�����ķ���
    ��д���ջPOPʱ����ջ��Ԫ��
*/


#ifndef STACK
#define STACK

#include <iostream>
#include <stack>

//ջ��ʹ��ģ�巺��
template<typename T>
class MyStack {
private:
    std::stack<T> s;

public:
    void push(const T& value) 
    {
        s.push(value);
    }

    T pop()         //��Ҫ�޸���pop����
    {
        if (s.empty()) 
        {
            throw runtime_error("ջΪ��");
        }
        T topValue = s.top();
        s.pop();
        return topValue;
    }

    T top() const   
    {
        if (s.empty()) 
        {
            throw runtime_error("ջΪ��");
        }
        return s.top();
    }

    bool empty() const 
    {
        return s.empty();
    }
};

#endif