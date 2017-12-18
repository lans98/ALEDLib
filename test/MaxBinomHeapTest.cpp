#include <memory>
#include "MaxBinomHeap.hpp"

int main() {
  auto heap = std::make_unique<qaed::MaxBinomHeap<int>>();

  heap->add(4);
  heap->add(9);
  heap->add(7);
  heap->add(1);
  heap->add(8);
  heap->add(6);
  heap->add(3);
  heap->add(3);
  heap->add(3);

  //heap->remove(2);

  //heap->removeTop();
  //heap->removeTop();
  //heap->removeTop();

  heap->graph("max_binom_heap.png", true);
  return 0;
}
