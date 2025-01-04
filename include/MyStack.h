#pragma once
#include <stack>
#include <stdexcept>

template<typename T>
class MyStack : public std::stack<T> {
public:
  T topAndPop();
};

template<typename T>
T MyStack<T>::topAndPop() {
  if (this->empty()) {
    throw std::out_of_range("Stack is empty");
  }
  T topElement = this->top();
  this->pop();
  return topElement;
}
