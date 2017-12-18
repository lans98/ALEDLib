#ifndef QAED_MAX_FIBO_HEAP_H
#define QAED_MAX_FIBO_HEAP_H 

#include "basic/BasicMaxMinHeap.hpp"
#include "tools/GVTools.hpp"

#include <list>
#include <cmath>
#include <queue>
#include <tuple>
#include <vector>
#include <sstream>
#include <algorithm>

namespace qaed {

template <class Type>
class MaxFiboHeap : public MaxHeap<Type> {
private:

  enum Color {
    WHITE, BLACK
  };

  // Node struct
  struct Node {
    using NodeContainer = typename std::list<Node*>;
    using NodeIterator  = typename std::list<Node*>::iterator;
    using Degree        = unsigned short;

  
    // Basic binomial node abstraction
    Degree degree;
    Type   data;
    Color  color;

    // Relations abstractions
    Node*         parent;
    NodeContainer children;
  
    // Constructors
    Node(Type&& data)      : degree(0), data(data), color(WHITE), parent(0), children() {}
    Node(const Type& data) : degree(0), data(data), color(WHITE), parent(0), children() {}

    // Iterators and useful access methods for children
    NodeIterator begin() { return children.begin(); }
    NodeIterator end()   { return children.end(); }

    std::size_t numOfChilds() { return children.size(); }

    Node*& get(size_t idx) {
      if (idx >= children.size())
        throw std::out_of_range("idx is greater than the number of children");

      NodeIterator it = children.begin();
      std::advance(it, idx);
      
      return *(it);
    }

  };

  using NodeContainer = typename Node::NodeContainer;
  using NodeIterator  = typename Node::NodeIterator;
  using NodeDegree    = typename Node::Degree;
  using SizeType      = std::size_t;

  // Main container, roots
  NodeContainer m_roots;
  NodeIterator  m_max;
  SizeType      m_size;

  bool isRoot(Node* n) { return (n->parent ? false : true); }

  NodeIterator basic_merge(NodeIterator& f, NodeIterator& s) {
    if ((*f)->data > (*s)->data) {
      (*f)->children.push_front(*s);
      (*s)->parent = *f;
      (*f)->degree += 1;
      m_roots.erase(s);
      return f;
    } else {
      (*s)->children.push_front(*f);
      (*f)->parent = *s;
      (*s)->degree += 1;
      m_roots.erase(f);
      return s;
    }
  }

  NodeIterator chain_merge(NodeIterator ii, std::vector<NodeIterator>& dg) {
    NodeDegree idx = (*ii)->degree;

    if (dg.at(idx) == ii || dg.at(idx) == m_roots.end()) {
      dg.at(idx) = ii;      
      return ii;
    } 
    
    NodeIterator tmp;

    tmp = dg.at(idx);
    tmp = basic_merge(ii, tmp);
    dg.at(idx) = m_roots.end();
    return chain_merge(tmp, dg);
  }

  void merge() {
    // So we don't create unnecesary variables
    if (m_roots.size() < 2) return;
  
    std::vector<NodeIterator> dg(std::ceil(std::log2(m_size)) + 1, m_roots.end());

    NodeIterator ii  = m_roots.begin();
    while (ii != m_roots.end()) {
      ii = chain_merge(ii, dg);
      ii = std::next(ii);
    }
  }

  void increaseKey(Node* n) {
    if (!n) return;

    Node* p = n->parent;  
    if (!p) return;

    if (n->data < p->data && n->color != BLACK) return;

    NodeIterator ii = std::find(p->begin(), p->end(), n);

    m_roots.push_front(n);
    p->children.erase(ii);
    p->degree -= 1;
    n->color = WHITE;

    if (p->color == WHITE) {
      if (p->parent)
        p->color = BLACK;

      return;
    } else {
      increaseKey(p);
    }
  }

  void updateMax() {
    NodeIterator ii = m_roots.begin();
    m_max           = ii;

    for (; ii != m_roots.end(); ++ii) {
      if ((*ii)->data > (*m_max)->data)
        m_max = ii;
    }
  }

  Node* find(const Type& data, NodeIterator& pos) {
    if (m_roots.empty() || data > (*m_max)->data)
      return 0;

    NodeIterator ii = m_roots.begin();
    Node*        el = 0;
    for (; ii != m_roots.end(); ++ii) {
      pos = ii;

      if (data > (*ii)->data)
        continue;
      else 
        el = find_internal(*ii, data);

      if (el) break;
    }

    return el;
  }

  Node* find_internal(Node* n, const Type& data) {
    if (n->data == data)     return n;
    if (n->children.empty()) return 0;

    NodeIterator ii = n->begin();
    Node*        el = 0;
    for (; ii != n->end(); ++ii) {
      if (data > (*ii)->data)
        continue;
      else 
        el = find_internal(*ii, data);

      if (el) break;
    }

    return el;
  }

public:
  MaxFiboHeap() : m_roots(), m_max(m_roots.end()), m_size(0) {}
 ~MaxFiboHeap() = default;

  void add(const Type& data) final {
    Node* tmp = new Node(data);
    m_roots.push_front(tmp);

    if (m_roots.size() == 1 || data > (*m_max)->data)
      m_max = m_roots.begin();

    m_size += 1;
  }

  void removeTop() final {
    if (m_roots.empty())
      return;
  
    Node* tmp = 0;
    while (!(*m_max)->children.empty()) {
      tmp = (*m_max)->children.back();
            (*m_max)->children.pop_back();
    
      tmp->parent = 0;
      m_roots.push_front(tmp);
    }

    delete (*m_max);
    m_roots.erase(m_max);
    merge();

    updateMax();

    m_size -= 1;
  }

  void remove(const Type& data) final {
    NodeIterator ii = m_roots.end();
    Node*        el = find(data, ii);
  
    if (!el) return;

    if (!el->parent) {
      (*ii)->data = (*m_max)->data + 1;
      m_max       = ii;
    } else {
      el->data = (*m_max)->data + 1;
      increaseKey(el);
      updateMax();
    }

    removeTop();
  }

  const Type& getMax() const final { 
    if (m_roots.empty())
      throw std::runtime_error("Empty heap");

    return (*m_max)->data; 
  }

  void sort() final { throw std::runtime_error("Can\'t be used to sort"); }

  void print(std::ostream& out = std::cout) const final {   }

  void graph(const std::string& filename, bool xdgopen = false) final { 
    if (m_roots.empty()) return;

    std::queue<Node*>                        nodes;
    std::queue<std::tuple<Node*, Agnode_t*>> parents;

    for (NodeIterator ii = m_roots.begin(); ii != m_roots.end(); ++ii) 
      nodes.push(*ii);

    std::stringstream converter;

    GVTool gvt;
    gvt.createGraph("G", Agdirected);
    gvt.defAttr(AGRAPH, "rank", "same");
    gvt.defAttr(AGNODE, "shape", "circle");
    gvt.setAttr(gvt.mainGraph(), "rank", "");
    Agraph_t* current_glevel = gvt.subgraph("");
    Agnode_t* current_agnode = 0;
    Agnode_t* cparent_agnode = 0;

    int no_nodes_currlvl = m_roots.size();
    int no_nodes_nextlvl = 0;

    std::tuple<Node*, Agnode_t*> parent_tuple;

    Node* temp_node   = 0;
    Node* temp_parent = 0;

    while (!nodes.empty()) {
      temp_node = nodes.front(); nodes.pop();

      no_nodes_nextlvl += temp_node->numOfChilds();
      no_nodes_currlvl -= 1;

      converter << temp_node->data;
      current_agnode = gvt.node(converter.str(), current_glevel, 0);

      if (!current_agnode) {
        current_agnode = gvt.node(converter.str(), current_glevel);

        std::get<0>(parent_tuple) = temp_node;
        std::get<1>(parent_tuple) = current_agnode;

      } else {
        std::string original = converter.str();
        for (int sub = 1; true; ++sub) {
          converter.str(std::string());
          converter.clear();

          converter << original;
          converter << "<SUB>" << sub << "</SUB>";

          gvt.addHTMLString(converter.str());

          current_agnode = gvt.node(converter.str(), current_glevel, 0);
          if (!current_agnode) {
            current_agnode = gvt.node(converter.str(), current_glevel);
            gvt.setAttr(current_agnode, "label", converter.str());

            std::get<0>(parent_tuple) = temp_node;
            std::get<1>(parent_tuple) = current_agnode;
            break;
          }
        }
      }

      converter.str(std::string());
      converter.clear();

      for (NodeIterator ii = temp_node->begin(); ii != temp_node->end(); ++ii) 
        nodes.push(*ii);

      if (!parents.empty() && temp_node->parent) {
        temp_parent    = std::get<0>(parents.front());
        cparent_agnode = std::get<1>(parents.front());

        while (temp_node->parent != temp_parent) {
          parents.pop();
          temp_parent    = std::get<0>(parents.front());
          cparent_agnode = std::get<1>(parents.front());
        }
        
        gvt.edge(cparent_agnode, current_agnode);
      }

      if (temp_node->children.size() > 0 || !temp_node->parent)
        parents.push(parent_tuple);
      
      if (no_nodes_nextlvl != 0 && no_nodes_currlvl == 0) {
        no_nodes_currlvl = no_nodes_nextlvl;
        current_glevel   = gvt.subgraph("");
        no_nodes_nextlvl = 0;
      }
    }

    gvt.layout("dot");
    gvt.renderToFile("png", filename.c_str());

    if (xdgopen) std::system(("xdg-open " + filename).c_str());
  }
};


}

#endif 
