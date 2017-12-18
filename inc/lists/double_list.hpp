#ifndef ALED_DOUBLE_LIST_H
#define ALED_DOUBLE_LIST_H

#include "list.h"

namespace aled {

template <class Type>
class DoubleList : public List<Type> {
private:

  struct Node {
    Type  data;
    Node* prev;
    Node* next;

    Node(Type&& data)      : data(data), prev(0), next(0) {}
    Node(const Type& data) : data(data), prev(0), next(0) {}
  };

  Node* m_head;

  bool __find__(const Type& data, Node** &p, Node** &q) {

    p = &m_head;
    q = 0;
    while (*p) {
      if ( (*p)->data == data) return true;
      if ( (*p)->data >  data) return false;

      q = p;
      p = &((*p)->next);
    }
    return false;
  }

public:
  DoubleList(): List<Type>(), m_head(0) {}

  bool add(const Type& data) final {

    Node** p;
    Node** q;

    if (!__find__(data, p, q)) {
      Node* tmp = new Node(data);
      tmp->next = *p;
      if ( q) tmp->prev  = *q;
      if (*p) (*p)->prev = tmp;
      *p = tmp;
      return true;
    }
    return false;
  }

  bool rmv(const Type& data) final {

    Node** p;
    Node** q;

    if (__find__(data, p, q)) {
      Node* tmp = *p;
      *p = (*p)->next;
      if (*p && q) (*p)->prev = *q;
      delete tmp;
      return true;
    }
    return false;

  }

#ifndef LIST_NO_PRINT
  void print(std::ostream& out) {
    for (Node* tmp = m_head; tmp; tmp = tmp->next)
      out << tmp->data << " => ";
    out << "END";
  }

  void printLn(std::ostream& out) {
    print(out);
    out << '\n';
  }
#endif

  friend class Iterator;

  class Iterator {
  private:
    DoubleList::Node* m_temp;
  public:
    Iterator(): m_temp(0) {}
    Iterator(DoubleList::Node* node): m_temp(node) {}
    Iterator(const Iterator& it): m_temp(it.m_temp) {}
    ~Iterator() {}

    Iterator& operator=(const Iterator& it) {
      m_temp = it.m_temp;
      return (*this);
    }

    Iterator& operator--() {
      if (!m_temp) throw std::runtime_error("iterator value is null");
      m_temp = m_temp->prev;
      return (*this);
    }

    Iterator& operator++() {
      if (!m_temp) throw std::runtime_error("iterator value is null");
      m_temp = m_temp->next;
      return (*this);
    }

    bool operator==(const Iterator& it) { return (m_temp == it.m_temp); }
    bool operator!=(const Iterator& it) { return (m_temp != it.m_temp); }

    Type& operator*() {
      if (!m_temp) throw std::runtime_error("iterator value is null");
      return m_temp->data;
    }

  };

  Iterator begin() const { return Iterator(this->m_head); }
  Iterator end()   const { return Iterator(0); }

  Iterator last() const {
    DoubleList::Node* tmp = this->m_head;
    while (tmp->next) tmp = tmp->next;
    return Iterator(tmp);
  }

  Iterator find(const Type& data) {
    DoubleList::Node** p;
    DoubleList::Node** q;

    if (__find__(data, p, q))
      return Iterator(*p);
    else
      return Iterator(0);
  }

};

}

#endif
