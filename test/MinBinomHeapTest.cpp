#include <memory>
#include "MinBinomHeap.hpp"

int main() {
  auto heap = std::make_unique<qaed::MinBinomHeap<int>>();

  heap->add(4);
  heap->add(2);
  heap->add(7);
  heap->add(1);
  heap->add(8);

  heap->remove(2);

  heap->removeTop();
  heap->removeTop();

  heap->add(9);
  heap->add(3);
  heap->add(6);

  heap->graph("min_binom_heap.png", true);

  return 0;
}
