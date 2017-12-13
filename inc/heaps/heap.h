#ifndef ALED_HEAP_H
#define ALED_HEAP_H 

#include <cstdlib>
#include <iostream>

namespace aled {

template <class Type>
class Heap {
public: 
  Heap()          = default;
  virtual ~Heap() = default;

  virtual void add(const Type& data)    = 0;
  virtual void removeTop()              = 0;
  virtual void remove(const Type& data) = 0;

  virtual void sort() = 0;

  virtual void print(std::ostream& out = std::cout) const = 0;
  virtual void graph(const std::string& filename, bool xdgopen = false) = 0;
};

}

#endif 
