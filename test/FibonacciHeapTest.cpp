#include <memory>
#include "FibonacciHeap.hpp"

int main() {
  auto max_heap = std::make_unique<qaed::MaxFibonacciHeap<int>>();

  max_heap->add(4);
  max_heap->add(2);
  max_heap->add(8);
  max_heap->add(1);
  max_heap->add(7);
  max_heap->remove_top();
  max_heap->remove(4);

  max_heap->add(9);
  max_heap->add(6);
  max_heap->add(5);

  max_heap->remove_top();

  max_heap->graph("max_fibonacci_heap.png", true);

  auto min_heap = std::make_unique<qaed::MinFibonacciHeap<int>>();

  min_heap->add(4);
  min_heap->add(2);
  min_heap->add(8);
  min_heap->add(1);
  min_heap->add(7);
  min_heap->remove_top();
  min_heap->remove(4);

  min_heap->add(9);
  min_heap->add(6);
  min_heap->add(5);

  min_heap->remove_top();

  min_heap->graph("min_fibonacci_heap.png", true);

  return 0;
}
