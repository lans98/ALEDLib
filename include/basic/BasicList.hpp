#ifndef ALED_LIST_H
#define ALED_LIST_H

#include <ostream>
#include <functional>

#include "tools/Sfinae.hpp"

namespace aled {

template <class Type>
class List {
protected:
  List() {
    static_assert(
      std::is_arithmetic<Type>::value        ||
      std::is_enum<Type>::value              ||
      std::is_pointer<Type>::value           ||
      std::is_same<Type, std::string>::value ||
      is_comparable<Type>::value      ,
      "Type is not an arithmetic, enum, pointer type nor string nor comparable object."
    );
  }

public:
  virtual ~List() = default;

  virtual bool add(const Type& data) = 0;
  virtual bool rmv(const Type& data) = 0;

#ifndef LIST_NO_PRINT
  virtual void print(std::ostream& out) = 0;
  virtual void printLn(std::ostream& out) = 0;
#endif
};

}

#endif
