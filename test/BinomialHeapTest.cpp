#include <memory>
#include "BinomialHeap.hpp"

int main() {
  auto max_heap = std::make_unique<qaed::MaxBinomialHeap<int>>();

  max_heap->add(4);
  max_heap->add(9);
  max_heap->add(7);
  max_heap->add(1);
  max_heap->add(8);
  max_heap->add(6);
  max_heap->add(3);
  max_heap->add(3);
  max_heap->add(3);

  max_heap->remove(2);

  max_heap->remove_top();
  max_heap->remove_top();
  max_heap->remove_top();

  max_heap->graph("max_binomial_heap.png", true);

  auto min_heap = std::make_unique<qaed::MinBinomialHeap<int>>();

  min_heap->add(4);
  min_heap->add(2);
  min_heap->add(7);
  min_heap->add(1);
  min_heap->add(8);

  min_heap->remove(2);

  min_heap->remove_top();
  min_heap->remove_top();

  min_heap->add(9);
  min_heap->add(3);
  min_heap->add(6);

  min_heap->graph("min_binomial_heap.png", true);
  
  return 0;
}
