#include <memory>
#include "heaps/min_fibo_heap.h"

int main() {
  auto heap = std::make_unique<aled::MinFiboHeap<int>>();

  heap->add(4);
  heap->add(2);
  heap->add(8);
  heap->add(1);
  heap->add(7);
  heap->removeTop();
  heap->remove(4);

  heap->add(9);
  heap->add(6);
  heap->add(5);

  heap->removeTop();

  heap->graph("min_fibo_heap.png", true);
  return 0;
}
