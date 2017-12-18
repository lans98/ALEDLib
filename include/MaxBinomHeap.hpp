#ifndef ALED_MAX_BINOM_HEAP_H
#define ALED_MAX_BINOM_HEAP_H 

#include "basic/BasicMaxMinHeap.hpp"
#include "tools/GVTools.hpp"

#include <set>
#include <list>
#include <queue>
#include <tuple>
#include <sstream>
#include <algorithm>

namespace aled {

template <class Type> 
class MaxBinomHeap : public MaxHeap<Type> {
private:

  // Node struct 
  struct Node {
    using NodeContainer = typename std::list<Node*>;
    using NodeIterator  = typename std::list<Node*>::iterator;
    using Degree        = unsigned short;

    // Basic binomial node abstraction
    Degree degree;
    Type   data;

    // Relations abstractions
    Node*         parent;
    NodeContainer children;

    // Constructors
    Node(Type&& data)      : degree(0), data(data), parent(0), children() {}
    Node(const Type& data) : degree(0), data(data), parent(0), children() {}


    // Iterators and useful access methods for children
    NodeIterator begin() { return children.begin(); }
    NodeIterator end()   { return children.end(); }
    
    std::size_t numOfChilds() { return children.size(); }

    Node*& get(size_t idx) {
      if (idx >= children.size()) 
        throw std::out_of_range("idx is greater than the number of childs");

      NodeIterator it = children.begin();
      std::advance(it, idx);
      
      return *(it);
    }
  };

  struct NodeComp {
    bool operator()(Node* l, Node* r) { return l->degree < r->degree; }
  };

  using RootContainer = typename std::multiset<Node*, NodeComp>;
  using RootIterator  = typename std::multiset<Node*, NodeComp>::iterator;

  using NodeContainer = typename Node::NodeContainer;
  using NodeIterator  = typename Node::NodeIterator;
  using NodeDegree    = typename Node::Degree;

  // Main container, roots
  RootContainer m_roots;
  RootIterator  m_max;

  int no_of_childs(Node* n) {
    if (!n) return -1;
    return n->children.size();
  }

  void basic_merge(RootIterator& f, RootIterator& s) {
    if ((*f)->data > (*s)->data) {
      (*f)->children.push_front(*s);
      (*s)->parent = *f;
      (*f)->degree += 1;
      m_roots.erase(s);
    } else {
      (*s)->children.push_front(*f);
      (*f)->parent = *s;
      (*s)->degree += 1;
      m_roots.erase(f);
    }
  }

  void merge() {
    // So we don't create unnecesary variables
    if (m_roots.size() < 2) return;

    // fit for First RootIterator
    // sit for Second RootIterator
    RootIterator fit;
    RootIterator sit;

    while (m_roots.size () >= 2) {
      fit = m_roots.begin();
      sit = std::next(fit);

      if ((*fit)->degree == (*sit)->degree) 
        basic_merge(fit, sit);
      else 
        return;
      
    }
  }

  Node* increaseKey(Node* n) {
    // n just for node
    if (!n) return 0;

    // p for parent
    Node* p = n->parent;
    if (!p) return n;

    while (p) {
      if (n->data > p->data)
        std::swap(n->data, p->data);
      else 
        return n;
      
      n = p;
      p = n->parent;
    }
    
    return n;
  }

  void updateMax() {
    RootIterator ii = m_roots.begin();
    m_max           = ii;
    for (; ii != m_roots.end(); ++ii) {
      if ((*ii)->data > (*m_max)->data)
        m_max = ii;
    }
  }

  Node* find(const Type& data, RootIterator& pos) {
    if (m_roots.empty() || data > (*m_max)->data) 
      return 0;

    RootIterator ii = m_roots.begin();
    Node*        el = 0;
    for(; ii != m_roots.end(); ++ii) {
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
  MaxBinomHeap() : m_roots(), m_max(m_roots.end()) {}
 ~MaxBinomHeap() = default;

  void add(const Type& data) final {
    Node* tmp = new Node(data);
    m_roots.insert(tmp);
  
    updateMax();

    merge();
  }

  void removeTop() final {
    if (m_roots.empty()) 
      return;

    Node* tmp = 0;
    while (!(*m_max)->children.empty()) {
      tmp = (*m_max)->children.back();
            (*m_max)->children.pop_back();
  
      tmp->parent = 0;
      m_roots.insert(tmp);
    }

    delete (*m_max);
    m_roots.erase(m_max);
    merge();

    updateMax();
  }

  void remove(const Type& data) final {
    RootIterator ii = m_roots.end();
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
  
  void sort() final { throw std::runtime_error("Can\'t be used to sort"); }

  const Type& getMax() const final { 
    if (m_roots.empty())
      throw std::runtime_error("Empty heap");

    return (*m_max)->data; 
  }

  void print(std::ostream& out = std::cout) const final {   }

  void graph(const std::string& filename, bool xdgopen = false) final { 
    if (m_roots.empty()) return;

    std::queue<Node*>                        nodes;
    std::queue<std::tuple<Node*, Agnode_t*>> parents;

    for (RootIterator ii = m_roots.begin(); ii != m_roots.end(); ++ii) 
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
