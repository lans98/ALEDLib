#include <memory>
#include "MaxArrayHeap.hpp"

int main() {
  std::vector<int> v { 8, 7, 3, 9, 19, 12, 14, 2, 1, 23 };

  std::unique_ptr<qaed::MaxHeap<int>> max_heap = std::make_unique<qaed::MaxArrayHeap<int>>(v);
  
  max_heap->print(); std::cout << std::endl;
  max_heap->removeTop();
  max_heap->print(); std::cout << std::endl;

  max_heap->sort();
  max_heap->print(); std::cout << std::endl;

  max_heap->graph("max_arr_heap.png", true);

  return 0; 
}
