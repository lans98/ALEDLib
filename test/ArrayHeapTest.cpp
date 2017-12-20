#include <memory>
#include "ArrayHeap.hpp"

int main() {
  std::vector<int> v { 8, 7, 3, 9, 19, 12, 5, 3, 2, 2, 3, 1, 17 };

  auto max_heap = std::make_unique<qaed::MaxArrayHeap<int>>(v);
  
  max_heap->print(); 
  std::cout << std::endl;
  max_heap->remove_top();
  max_heap->print(); 
  std::cout << std::endl;

  max_heap->sort();
  max_heap->print(); std::cout << std::endl;

  max_heap->graph("max_array_heap.png", true);

  auto min_heap = std::make_unique<qaed::MinArrayHeap<int>>(v);
  
  min_heap->print(); 
  std::cout << std::endl;
  min_heap->remove_top();
  min_heap->print(); 
  std::cout << std::endl;

  min_heap->sort();
  min_heap->print(); 
  std::cout << std::endl;

  min_heap->graph("min_array_heap.png", true);

  return 0;
}
