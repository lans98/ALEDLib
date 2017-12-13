#ifndef ALED_MIN_ARRAY_HEAP
#define ALED_MIN_ARRAY_HEAP

#include "max_min_heap.h"
#include "graphviz_tools/gvtools.h"

#include <queue>
#include <vector>
#include <sstream>
#include <algorithm>

namespace aled {

template <class Type>
class MinArrayHeap : public MinHeap<Type> {
private:
  std::vector<Type> m_vec;

  using idx_t   = typename std::vector<Type>::size_type;
  using vecit_t = typename std::vector<Type>::iterator;

  idx_t left(const idx_t& ii)   { return (ii * 2) + 1; }
  idx_t right(const idx_t& ii)  { return left(ii) + 1; }
  idx_t parent(const idx_t& ii) { return (ii - 1) / 2; }


public:
  MinArrayHeap() = default;
 ~MinArrayHeap() = default;

  MinArrayHeap(const std::size_t& reserve): MinHeap<Type>(), m_vec(reserve) {}
  MinArrayHeap(const std::vector<Type>& vec): MinHeap<Type>(), m_vec(vec) {
    for (idx_t ii = m_vec.size()/2; ii != -1UL; --ii)
      heapify(ii);
  }

  void add(const Type& data) final {
    m_vec.push_back(data);
    heapify(parent(m_vec.size() - 1));
  }

  void removeTop() final {
    if (m_vec.empty()) return;

    std::swap(m_vec.at(0), m_vec.at(m_vec.size() - 1));
    m_vec.pop_back();
    heapify(0);
  }

  void remove(const Type& data) final {
    idx_t ii =  find(data);
    m_vec.at(ii) = m_vec.at(0) - 1;
    decreaseKey(ii);
    removeTop();
  }

  void sort() final {
    size_t size = m_vec.size();
    while (size > 1) {
      std::swap(m_vec.at(0), m_vec.at(size - 1));
      heapify(0, --size);
    }

    std::reverse(m_vec.begin(), m_vec.end());
  }

  const Type& getMin() const final {
    if (m_vec.empty())
      throw std::runtime_error("Empty heap");

    return m_vec.at(0);
  }

  void print(std::ostream& out) const final {
    for (auto& x : m_vec)
      out << x << ' ';
  }

  void graph(const std::string& filename, bool xdgopen = false) final {
    if (m_vec.empty()) return;

    std::queue<idx_t>     nodes;
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
    idx_t temp_node;

    while (!nodes.empty()) {
      // Extract the top element
      temp_node = nodes.front(); nodes.pop();

      // Increase the number of childs of the next level
      // and decrease the actual nodes processed in the
      // current level
      no_nodes_nextlvl += noOfChilds(temp_node);
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
      if (noOfChilds(temp_node) != 0)
        parents.push(current_agnode);

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

private:

  int noOfChilds(const idx_t& ii) {
    int num = 0;

    if (left(ii) < m_vec.size())
      num += 1;

    if (right(ii) < m_vec.size())
      num += 1;

    return num;
  }

  idx_t find(const Type& data) {
    vecit_t it = std::find(m_vec.begin(), m_vec.end(), data);
    return std::distance(m_vec.begin(), it);
  }

  void heapify(const idx_t& ii, size_t size = 0) {
    if (size == 0)
      size = m_vec.size();
    else if (size > m_vec.size())
      return;

    if (ii + 1 > size)       return;
    if (left(ii) + 1 > size) return;

    idx_t m = ii;
    if (m_vec.at(ii) > m_vec.at(left(ii)))
      m = left(ii);

    if (right(ii) < size && m_vec.at(m) > m_vec.at(right(ii)))
      m = right(ii);

    if (m == ii) return;

    std::swap(m_vec.at(ii), m_vec.at(m));
    heapify(m, size);
  }

  void decreaseKey(const idx_t& ii) {
    idx_t m = ii;

    while (m != 0) {
      if (m_vec.at(m) > m_vec.at(parent(m)))
        std::swap(m_vec.at(m), m_vec.at(parent(m)));
      else
        return;

      m = parent(ii);
    }

  }

};


}

#endif
