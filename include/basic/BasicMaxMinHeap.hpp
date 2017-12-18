#ifndef ALED_MAX_MIN_HEAP_H
#define ALED_MAX_MIN_HEAP_H 

#include "BasicHeap.hpp"

namespace aled {

template <class Type>
class MaxHeap : public Heap<Type> {
public:
  MaxHeap()          = default;
  virtual ~MaxHeap() = default;

  virtual const Type& getMax() const = 0;
};

template <class Type> 
class MinHeap : public Heap<Type> {
public: 
  MinHeap()          = default;
  virtual ~MinHeap() = default;

  virtual const Type& getMin() const = 0;
};

}

#endif 
