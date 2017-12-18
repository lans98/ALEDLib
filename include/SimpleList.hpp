#ifndef ALED_SIMPLE_LIST_H
#define ALED_SIMPLE_LIST_H

#include "basic/BasicList.hpp"

namespace aled {

template <class Type>
class SimpleList : public List<Type> {
private:

  // Basic block
  struct Node {
    Type  data;
    Node* next;

    Node(Type&& data)      : data(data), next(nullptr) {}
    Node(const Type& data) : data(data), next(nullptr) {}
  };

  // Starting node
  Node* m_head;

public:
  SimpleList(): List<Type>(), m_head(nullptr) {}
 ~SimpleList() {
    Node* tmp;

    while (m_head) {
      tmp    = m_head;
      m_head = m_head->next;
      delete tmp;
    }
  }

  bool add(const Type& data) final {
    Node** p;

    if (!find(data, p)) {
      Node* tmp = new Node(data);
      tmp->next = (*p);
      (*p)      = tmp;

      return true;
    }
    return false;
  }

  bool rmv(const Type& data) final {
    Node** p;

    if (find(data, p)) {
      Node* tmp = (*p);
      (*p)      = (*p)->next;

      delete tmp;
      return true;
    }
    return false;
  }

#ifndef LIST_NO_PRINT
  void print(std::ostream& out) final {
    for (Node* tmp = m_head; tmp; tmp = tmp->next)
      out << tmp->data << " => ";
    out << "END";
  }

  void printLn(std::ostream& out) final {
    print(out);
    out << '\n';
  }
#endif

private:

  bool find(const Type& data, Node**& p) {
    p = &m_head;

    while (*p) {
      if ((*p)->data == data) return true;
      if ((*p)->data >  data) return false;

      p = &((*p)->next);
    }
    return false;
  }

public:

  friend class Iterator;

  class Iterator {
  private:
    SimpleList::Node* m_temp;
  public:
    Iterator(): m_temp(0) {}
    Iterator(SimpleList::Node* node): m_temp(node) {}
    Iterator(const Iterator& it): m_temp(it.m_temp) {}
    ~Iterator() {}

    Iterator& operator=(const Iterator& it) {
      m_temp = it.m_temp;
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
    SimpleList::Node* tmp = this->m_head;

    while (tmp->next)
      tmp = tmp->next;
    return Iterator(tmp);
  }

  Iterator find(const Type& data) {
    SimpleList::Node** p;
    if (find(data, p))
      return Iterator(*p);
    else
      return Iterator(0);
  }

};

}

#endif
