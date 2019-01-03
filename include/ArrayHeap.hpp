#ifndef QAED_ARRAY_HEAP_HPP
#define QAED_ARRAY_HEAP_HPP 

#include "tools/GVTools.hpp"
#include "basic/BasicHeap.hpp"

#include <queue>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>

namespace qaed {

template <class Type, class Comp, class Modi>
class ArrayHeap : public Heap<Type> {
public:
  using Vector = std::vector<Type>;
  using Itr = typename std::vector<Type>::iterator;
  using Idx = typename std::vector<Type>::size_type;
  using Size = Idx;

  using value_type = Type;
  using reference = Type&;
  using const_reference = Type const&;

private:
  Vector m_vec;
  Comp comp;
  Modi modi;

  struct Equal {
    bool operator()(const Type& l, const Type& r) {
      return !(Comp()(l,r) || Comp()(r,l));
    }
  } equal;

public:
  ArrayHeap() = default;
 ~ArrayHeap() = default;

  ArrayHeap(const Vector& vec) : Heap<Type>(), m_vec(vec) {
    for (Idx ii = m_vec.size()/2; ii != -1UL; --ii)
      heapify(ii);
  }

  void add(const Type& data) noexcept(false) final {
    m_vec.push_back(data);
    heapify(parent(m_vec.size() - 1));
  }

  void remove(const Type& data) noexcept(false) final {
    Idx ii       = find(data);
    m_vec.at(ii) = modi(m_vec.at(0), Type(1));
    bubble_key(ii);
    remove_top();
  }

  void remove_top() noexcept(false) final {
    if (m_vec.empty()) 
      throw std::logic_error("Heap is empty");

    std::swap(m_vec.at(0), m_vec.at(m_vec.size() - 1));
    m_vec.pop_back();
    heapify(0);
  }

  const_reference get_top() noexcept(false) final {
    if (m_vec.empty())
      throw std::logic_error("Heap is empty");

    return m_vec.at(0);
  }

  Size size() final { return m_vec.size(); }
  bool empty() final { return m_vec.empty(); }

  void sort() {
    Size size = m_vec.size();
    while (size > 1) {
      std::swap(m_vec.at(0), m_vec.at(size - 1));
      heapify(0, --size);
    }
    std::reverse(m_vec.begin(), m_vec.end());
  }

  const Vector& get_vector() { return m_vec; }

  void print(std::ostream& out = std::cout) const final {
    for (auto& x : m_vec)
      out << x << ' ';
  }

  void graph(const char* filename, bool xdgopen = false) final {
    if (m_vec.empty()) return;

    std::queue<Idx>       nodes;
    std::queue<Agnode_t*> parents;
    nodes.push(0);

    std::stringstream converter;

    GVTool gvt;
    gvt.createGraph("G", Agdirected);
    gvt.defAttr(AGRAPH, "rank", "same");
    gvt.defAttr(AGNODE, "shape", "circle");
    gvt.setAttr(gvt.mainGraph(), "rank", "");
    Agraph_t* current_glevel = gvt.subgraph("");
    Agnode_t* current_agnode = 0;
    Agnode_t* cparent_agnode = 0;

    int no_nodes_nextlvl = 0;
    int no_nodes_currlvl = 1;
    int node_degree      = 0;
    Idx temp_node;

    while (!nodes.empty()) {
      // Extract the top element
      temp_node = nodes.front(); nodes.pop();

      // Increase the number of childs of the next level
      // and decrease the actual nodes processed in the 
      // current level
      no_nodes_nextlvl += no_of_childs(temp_node);
      no_nodes_currlvl -= 1;
      
      // Convert data from the actual node to string
      converter << m_vec.at(temp_node);

      // Only search for node
      current_agnode = gvt.node(converter.str(), current_glevel, 0);

      // If the search returns null create node 
      if (!current_agnode) {
        current_agnode = gvt.node(converter.str(), current_glevel);
      } else {
        // Otherwise we found a node, then try to create another
        // one with with a subscript
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
            break;
          }
        }
      }

      converter.str(std::string());
      converter.clear();

      // If there are left and right childs push them to the 
      // 'nodes' queue.
      if (left(temp_node) < m_vec.size())
        nodes.push(left(temp_node));

      if (right(temp_node) < m_vec.size())
        nodes.push(right(temp_node));

      // If 'parents' queue is not empty, then the top element is a 
      // father
      if (!parents.empty()) {
        cparent_agnode = parents.front(); 
        node_degree    = agdegree(gvt.mainGraph(), cparent_agnode, false, true);

        // If the degree of cparent_agnode is greater or equal to 2
        // pop this node from the queue and select the next one to 
        // be the parent
        if (node_degree >= 2) {
          parents.pop();
          cparent_agnode = parents.front();
        }

        // Finally create an edge between the actual node an the 
        // parent node
        gvt.edge(cparent_agnode, current_agnode);
      }

      // Just push the actual node if it has at least one child 
      // that is, it's a parent
      if (no_of_childs(temp_node) != 0)
        parents.push(current_agnode);

      if (no_nodes_nextlvl != 0 && no_nodes_currlvl == 0) {
        no_nodes_currlvl = no_nodes_nextlvl;
        current_glevel   = gvt.subgraph("");
        no_nodes_nextlvl = 0;
      }
    }

    gvt.layout("dot");
    gvt.renderToFile("png", filename);

    if (xdgopen) {
      std::string cmd = "xdg-open ";
      std::system(cmd.append(filename).c_str());
    }
  }
  
private:
  Idx left(const Idx& ii) noexcept { return (ii * 2) + 1; }
  Idx right(const Idx& ii) noexcept { return left(ii) + 1; }
  Idx parent(const Idx& ii) noexcept { return (ii - 1) / 2; }

  int no_of_childs(const Idx& ii) noexcept {
    int num = 0;
    if (left(ii) < m_vec.size())
      num += 1;
    if (right(ii) < m_vec.size())
      num += 1;
    return num;
  }

  Idx find(const Type& data) noexcept(false) {
    Itr ii = std::find_if(m_vec.begin(), m_vec.end(), [&data, this](auto& e){ 
      return this->equal(e, data);
    });

    if (ii == m_vec.end())
      throw std::runtime_error("Element doesn't exist");

    return std::distance(m_vec.begin(), ii);
  }

  void heapify(const Idx& ii, Size size = 0) noexcept(false) {
    if (size == 0)
      size == m_vec.size();
    else if (size > m_vec.size())
      throw std::logic_error("size passed is bigger than array size");

    if (ii + 1 > size || left(ii) + 1 > size) 
      return;

    Idx m = ii;
    if (comp(m_vec.at(ii), m_vec.at(left(ii))))
      m = left(ii);

    if (right(ii) < size && comp(m_vec.at(m), m_vec.at(right(ii))))
      m = right(ii);

    if (m == ii) return;
    std::swap(m_vec.at(ii), m_vec.at(m));
    heapify(m, size);
  }

  void bubble_key(const Idx& ii) noexcept(false) {
    Idx m = ii;

    for (Idx m = ii; m != 0; m = parent(m)) {
      if (!comp(m_vec.at(m), m_vec.at(parent(m))))
        std::swap(m_vec.at(m), m_vec.at(parent(m)));
      else
        return;
    }
  }
};

template <class Type>
using MaxArrayHeap = ArrayHeap<Type, std::greater<Type>, std::plus<Type>>;

template <class Type>
using MinArrayHeap = ArrayHeap<Type, std::less<Type>, std::minus<Type>>;

}

#endif
