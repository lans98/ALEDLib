#ifndef QAED_BINOMIAL_HEAP_HPP
#define QAED_BINOMIAL_HEAP_HPP

#include "tools/GVTools.hpp"
#include "basic/BasicHeap.hpp"

#include <list>
#include <cmath>
#include <tuple>
#include <queue>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>

namespace qaed {

template <class Type, class Comp, class Modi >
class BinomialHeap : public Heap<Type> {
public:
  using value_type = Type;
  using reference = Type&;
  using const_reference = Type const&;
  using Size = std::size_t;

private:

  struct Node {
    using NodeCont = typename std::list<Node*>;
    using NodeItr  = typename std::list<Node*>::iterator;
    using Degree   = unsigned short;

    Type   data;
    Degree degree;

    Node*    parent;
    NodeCont children;

    Node(const_reference data) : degree(0), data(data), parent(0), children() {}

    NodeItr beg() { return children.begin(); }
    NodeItr end() { return children.end(); }

    Size num_of_children() { return children.size(); }

    Node*& at(Size ii) {
      if (ii >= children.size())
        throw std::out_of_range("range overflow");

      NodeItr it = children.begin();
      std::advance(it, ii);
      return *it;
    }
  };

  using NodeCont   = typename Node::NodeCont;
  using NodeItr    = typename Node::NodeItr;
  using NodeDegree = typename Node::Degree;

  NodeCont m_roots;
  NodeItr  m_top;
  Size     m_size;

  Comp comp;
  Modi modi;

  struct Equal {
    bool operator()(const Type& l, const Type& r) {
      return !(Comp()(l, r) || Comp()(r, l));
    }
  } equal;

public:
  BinomialHeap() : m_roots(), m_top(m_roots.end()) {}
 ~BinomialHeap() = default;

  void add(const_reference data) final {
    Node* tmp = new Node(data);
    m_roots.push_front(tmp);
    merge();
    update_top();
  }

  void remove(const_reference data) final {
    NodeItr ii  = m_roots.end();
    Node*   tmp = find(data, ii);

    if (!tmp) return;

    if (is_root(tmp)) {
      (*ii)->data = modi((*m_top)->data, Type(1));
      m_top = ii;
    } else {
      tmp->data = modi((*m_top)->data, Type(1));
      bubble_key(tmp);
      update_top();
    }

    remove_top();
  }

  void remove_top() final {
    if (m_roots.empty()) return;
    
    Node* tmp = nullptr;
    while (!(*m_top)->children.empty()) {
      tmp = (*m_top)->children.back();
            (*m_top)->children.pop_back();
      tmp->parent = nullptr;
      m_roots.push_front(tmp);
    }

    delete (*m_top);
    m_roots.erase(m_top);
    merge();
    update_top();
    m_size -= 1;
  }

  const_reference get_top() final {
    if (m_roots.empty() || m_top == m_roots.end())
      throw std::runtime_error("Heap is empty");

    return (*m_top)->data; 
  }

  Size size() final { return m_size; }
  bool empty() final { return m_size == 0; }

  void print(std::ostream& out = std::cout) const final {  }

  void graph(const char* filename, bool xdgopen = false) final {
    if (m_roots.empty()) return;

    std::queue<Node*>                        nodes;
    std::queue<std::tuple<Node*, Agnode_t*>> parents;

    for (NodeItr ii = m_roots.begin(); ii != m_roots.end(); ++ii) 
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

      no_nodes_nextlvl += temp_node->num_of_children();
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

      for (NodeItr ii = temp_node->beg(); ii != temp_node->end(); ++ii) 
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
    gvt.renderToFile("png", filename);

    if (xdgopen) {
      std::string cmd("xdg-open ");
      std::system(cmd.append(filename).c_str());
    }
  }

private:
  bool is_root(Node* n) { return (bool) !n->parent; }

  NodeItr basic_merge(NodeItr& f, NodeItr& s) {
    if (comp((*f)->data, (*s)->data)) {
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

  NodeItr chain_merge(NodeItr it, std::vector<NodeItr>& dg) {
    NodeDegree id = (*it)->degree;

    if (dg.at(id) == it || dg.at(id) == m_roots.end()) {
      dg.at(id) = it;
      return it;
    }

    NodeItr tmp;
    tmp = dg.at(id);
    tmp = basic_merge(it, tmp);
    dg.at(id) = m_roots.end();
    return chain_merge(tmp, dg);
  }

  void merge() {
    if (m_roots.size() < 2) return;

    std::vector<NodeItr> dg(std::ceil(std::log2(m_size) + 1), m_roots.end());
    
    NodeItr ii = m_roots.begin();
    while (ii != m_roots.end()) {
      ii = chain_merge(ii, dg);
      ii = std::next(ii);
    }
  }

  void update_top() {
    if (m_roots.empty()) {
      m_top = m_roots.end();
      return;
    }

    if (m_roots.size() == 1) {
      m_top = m_roots.begin();
      return;
    } 

    NodeItr ii = m_roots.begin();
    for (m_top = ii; ii != m_roots.end(); ++ii) {
      if (comp((*ii)->data, (*m_top)->data))
        m_top = ii;
    }
  }

  void bubble_key(Node* n) {
    if (!n) return;
    Node* p = n->parent;
    if (!p) return;

    while (p) {
      if (!comp(n->data, p->data))
        std::swap(n->data, p->data);
      else 
        return;

      n = p;
      p = n->parent;
    }
  }

  Node* find(const_reference data, NodeItr& found_at) {
    if (m_roots.empty() || comp(data, (*m_top)->data))
      return nullptr;

    Node* tmp;
    for (NodeItr ii = m_roots.begin(); ii != m_roots.end(); ++ii) {
      found_at = ii;
      if (comp(data, (*ii)->data))
        continue;
      else 
        tmp = find_internal(*ii, data);

      if (tmp) break;
    }

    return tmp;
  }

  Node* find_internal(Node* n, const_reference data) {
    if (equal(n->data, data)) return n;
    if (n->children.empty())  return nullptr;

    Node* tmp;
    for (NodeItr ii = n->beg(); ii != n->end(); ++ii) {
      if (comp(data, (*ii)->data))
        continue;
      else 
        tmp = find_internal(*ii, data);

      if (tmp) break;
    }

    return tmp;
  }
};

template <class Type>
using MaxBinomialHeap = BinomialHeap<Type, std::greater<Type>, std::plus<Type>>;

template <class Type>
using MinBinomialHeap = BinomialHeap<Type, std::less<Type>, std::minus<Type>>;

}

#endif
