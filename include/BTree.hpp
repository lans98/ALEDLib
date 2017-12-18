#ifndef QAED_B_TREE_H
#define QAED_B_TREE_H 

#include <array>
#include <vector>

#include "tools/Sfinae.hpp"

namespace qaed {

template <class Type, int PageSize>
class BTree {
private:

  struct Page;

  using TypeArray = typename std::vector<Type>;
  using PageArray = typename std::vector<Page*>;

  struct Page {
  private:
    TypeArray m_data;
    PageArray m_child;

  public:

    Page() : m_data(), m_child() {
      m_data.reserve(PageSize);
      m_child.reserve(PageSize + 1);
    }

    Type& operator[](const int& idx) { return m_data.at(idx); }
    Page* operator()(const int& idx) { return m_child.at(idx);  }

  };

  Page*     m_root;
  TypeArray m_buff;

public:
  BTree() : m_root(0), m_buff() { m_buff.reserve(PageSize + 1); }

};


}

#endif
