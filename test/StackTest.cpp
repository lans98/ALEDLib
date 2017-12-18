#include <iostream>
#include <memory>

#include "Stack.hpp"

int main() {
  auto stack = std::make_unique<aled::Stack<int>>();

  stack->push(5);
  stack->push(7);
  stack->push(9);
  stack->push(2);
  stack->push(1);

  while(!stack->isEmpty()) {
    std::cout << stack->top() << ' ';
    stack->pop();
  } std::cout << std::endl;

  return 0;
}
