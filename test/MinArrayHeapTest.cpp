#include <memory>
#include "MinArrayHeap.hpp"

int main() {
  std::vector<int> v { 8, 7, 3, 9, 19, 12, 5, 3, 2, 2, 3, 1, 17 };

  std::unique_ptr<aled::MinHeap<int>> min_heap = std::make_unique<aled::MinArrayHeap<int>>(v);
  
  min_heap->print(); std::cout << std::endl;
  min_heap->removeTop();
  min_heap->print(); std::cout << std::endl;

  min_heap->sort();
  min_heap->print(); std::cout << std::endl;

  min_heap->graph("min_arr_heap.png", true);

  return 0; 
}
