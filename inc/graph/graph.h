#ifndef ALED_GRAPH_H
#define ALED_GRAPH_H

#include <set>
#include <queue>
#include <stack>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <type_traits>

#include "sfinae.h"
#include "heaps/min_fibo_heap.h"

namespace aled {

enum G_TYPE {
  DIRECTED,
  UNDIRECTED
};

template <class VertexTag, class EdgeTag, G_TYPE type>
class Graph {
private:

  struct Edge;

  using EdgeSet = typename std::set<Edge>;
  using EdgeItr = typename std::set<Edge>::iterator;

  struct Vertex {
  private:
    VertexTag       m_data;
    mutable bool    m_mark;
    mutable EdgeSet m_edges;

  public:
    Vertex() = default;
    Vertex(const VertexTag& data) : m_data(data), m_mark(false), m_edges() {}
    Vertex(const Vertex& v) : m_data(v.m_data), m_mark(v.m_mark), m_edges(v.m_edges) {}

    bool operator==(const Vertex& v) const { return m_data == v.m_data; }
    bool operator!=(const Vertex& v) const { return m_data != v.m_data; }
    bool operator<(const Vertex& v)  const { return m_data < v.m_data; }
    bool operator>(const Vertex& v)  const { return m_data > v.m_data; }
    bool operator<=(const Vertex& v) const { return m_data <= v.m_data; }
    bool operator>=(const Vertex& v) const { return m_data >= v.m_data; }

    void mark()   const { m_mark = true; }
    void unmark() const { m_mark = false; }
    bool marked() const { return m_mark; }
    EdgeSet& edges() const { return m_edges; }

    const VertexTag& get_data() const { return m_data; }

    friend std::ostream& operator<<(std::ostream& os, const Vertex& v) {
      os << "[" << v.m_data << "]";
      return os;
    }
  };

  friend std::ostream& operator<<(std::ostream& out, const EdgeSet& e) {
    for (EdgeItr ii = e.begin(); ii != e.end(); ++ii) 
      std::cout << "(" << *ii << ", " << ii->dest() << ")" ;
  }

  using VertexSet = typename std::set<Vertex>;
  using VertexItr = typename std::set<Vertex>::iterator;

  struct Edge {
  private: 
    VertexItr       m_dest;
    mutable EdgeTag m_data;

  public:
    Edge() = default;
    Edge(const EdgeTag& data) : m_dest(), m_data(data) {}
    Edge(const VertexItr& m_dest, const EdgeTag& m_data = EdgeTag()) : m_dest(m_dest), m_data(m_data) {}
    Edge(const Edge& e) : m_dest(e.m_dest), m_data(e.m_data) {}

    bool operator==(const Edge& e) const { return *m_dest == *e.m_dest; }
    bool operator!=(const Edge& e) const { return *m_dest != *e.m_dest; }
    bool operator<(const Edge& e)  const { return *m_dest <  *e.m_dest; }
    bool operator>(const Edge& e)  const { return *m_dest >  *e.m_dest; }
    bool operator<=(const Edge& e) const { return *m_dest <= *e.m_dest; }
    bool operator>=(const Edge& e) const { return *m_dest >= *e.m_dest; }

    Edge& operator-(const int& e) { }

    const VertexItr& vertex_itr() const { return m_dest; }
    const Vertex& vertex() const { return *m_dest; }
    const EdgeTag& get_tag() const { return m_data; }
    void  set_tag(const EdgeTag& et) const { m_data = et; }

    friend std::ostream& operator<<(std::ostream& os, const Edge& e) {
      os << "(" << e.m_data << ", " << *e.m_dest << ")";
      return os;
    }

    struct tagless {
      bool operator()(const Edge& a, const Edge& b) { return a.m_data < b.m_data; }
      bool operator()(const EdgeItr& a, const EdgeItr& b) { return a->get_tag() < b->get_tag(); }
    };

    struct tagequal {
      bool operator()(const Edge& a, const Edge& b) { return a.m_data == b.m_data; }
      bool operator()(const EdgeItr& a, const EdgeItr& b) { return a->get_tag() == b->get_tag(); }
    };

    struct tagminus {
      EdgeTag operator()(const Edge& a, const Edge& b) { return a.m_data - b.m_data; }
      EdgeTag operator()(const EdgeItr& a, const EdgeItr& b) { return a->get_tag() - b->get_tag(); }
    };
  };

  struct FullyEdge {
  private:
    VertexItr m_beg;
    VertexItr m_end;
    EdgeTag   m_data;

  public:
    FullyEdge() = default;
    FullyEdge(const EdgeTag& data): m_beg(), m_end(), m_data() {}

    FullyEdge(const VertexItr& b, const VertexItr& e, const EdgeTag& d):
      m_beg(b),
      m_end(e),
      m_data(d) {}

    FullyEdge(const FullyEdge& fe) : 
      m_beg(fe.m_beg),
      m_end(fe.m_end),
      m_data(fe.m_data) {}

    bool operator==(const Edge& e) const { return m_data == e.m_data; }
    bool operator!=(const Edge& e) const { return m_data != e.m_data; }
    bool operator<(const Edge& e)  const { return m_data <  e.m_data; }
    bool operator>(const Edge& e)  const { return m_data >  e.m_data; }
    bool operator<=(const Edge& e) const { return m_data <= e.m_data; }
    bool operator>=(const Edge& e) const { return m_data >= e.m_data; }

    FullyEdge operator-(const FullyEdge& e) { FullyEdge t(*this); t.m_data -= e.m_data; return t; }

    const Vertex& beg() const { return *m_beg; }
    const Vertex& end() const { return *m_end; }

    const VertexItr& beg_itr() const { return m_beg; }
    const VertexItr& end_itr() const { return m_end; }

    const EdgeTag& get_tag() const { return m_data; }
    void set_tag(const EdgeTag& d) { return m_data = d; }

    operator Edge() { return Edge(m_end, m_data); }
  };

  VertexSet m_g;
  std::size_t m_no_vertexes;
  std::size_t m_no_edges;

public:
  Graph() : m_g(), m_no_vertexes(0), m_no_edges(0) {

    static_assert(
      type == DIRECTED   ||
      type == UNDIRECTED ,
      "Unknown graph type (posible values are {aled::DIRECTED, aled::UNDIRECTED})"      
    );

    static_assert(
      std::is_arithmetic<VertexTag>::value        ||
      std::is_same<VertexTag, std::string>::value ||
      is_comparable<VertexTag>::value             ,
      "VertexTag is not an arithmetic type nor string nor comparable object."
    );

    static_assert(
      std::is_arithmetic<EdgeTag>::value        ||
      std::is_same<EdgeTag, std::string>::value ||
      is_comparable<EdgeTag>::value             ,
      "EdgeTag is not an arithmetic type nor string nor comparable object."
    );
  }

  std::size_t no_vertexes() { return m_no_vertexes; }
  std::size_t no_edges() { return m_no_edges; }

  auto add_vertex(const VertexTag& data) {
    auto result = m_g.emplace(Vertex(data)); 
    if (result.second)
      m_no_vertexes += 1;

    return result;
  }

  auto add_vertex(const Vertex& v) {
    auto result = m_g.insert(v); 
    if (result.second)
      m_no_vertexes += 1;

    return result;
  }

  auto add_edge(const VertexTag& d1, const VertexTag& d2, const EdgeTag& data) {
    VertexItr i1 = m_g.find(Vertex(d1));
    VertexItr i2 = m_g.find(Vertex(d2));
    return add_edge(i1, i2, data);
  }

  auto add_edge(const VertexItr& v1, const VertexItr& v2, const EdgeTag& data) {
    if (v1 == m_g.end() || v2 == m_g.end()) 
      throw std::runtime_error("One/two vertex(s) were not found");

    auto result = v1->edges().emplace(Edge(v2, data));
    if (result.second)
      m_no_edges += 1;

    if constexpr (type == UNDIRECTED) 
      result = v2->edges().emplace(Edge(v1, data));

    return result;
  }

  bool remove_vertex(const VertexTag& data) {
    VertexItr it = m_g.find(Vertex(data));
    return remove_vertex(it);
  }

  bool remove_vertex(const VertexItr& v) {
    if (v == m_g.end()) return false;

    remove_edges_with(v);
    m_g.erase(v);
    m_no_vertexes -= 1;

    return true;
  }

  bool remove_edge(const VertexTag& d1, const VertexTag& d2) {
    VertexItr i1 = m_g.find(Vertex(d1));
    VertexItr i2 = m_g.find(Vertex(d2));
    return remove_edge(i1, i2);
  }

  bool remove_edge(const VertexItr& v1, const VertexItr& v2) {
    if (v1 == m_g.end() || v2 == m_g.end()) return false;

    EdgeItr e = v1->edges().find(Edge(v2));
    if (e != v1->edges().end()) {
      v1->edges().erase(e);
      m_no_edges -= 1;
    }

    if constexpr (type == UNDIRECTED) {
      e = v2->edges().find(Edge(v1));
      if (e != v2->edges().end()) 
        v2->edges().erase(e);
    }


    return true;
  }

  bool remove_edges_with(const VertexTag& d) {
    return remove_edges_with(m_g.find(Vertex(d))); 
  }

  bool remove_edges_with(const VertexItr& vit) {
    if (vit == m_g.end()) return false;
    vit->edges().clear();

    EdgeItr tmp;
    for (VertexItr ii = m_g.begin(); ii != m_g.end(); ++ii) {
      if (ii == vit) continue;

      tmp = ii->edges().find(Edge(vit));
      if (tmp != ii->edges().end()) {
        ii->edges().erase(tmp);
        m_no_edges -= 1;
      }
    }

    return true;
  }

  EdgeTag const& get_tag_edge(const VertexTag& a, const VertexTag& b) {
    VertexItr ia = m_g.find(Vertex(a));
    VertexItr ib = m_g.find(Vertex(b));

    return get_tag_edge(ia, ib);
  }

  EdgeTag const& get_tag_edge(const VertexItr& a, const VertexItr& b) {
    if (a == m_g.end() || b == m_g.end())
      throw std::runtime_error("One (or two) vertex(s) were not found");

    EdgeItr edge;
    if constexpr (type == DIRECTED) {
      edge = a->edges().find(Edge(b));
      if (edge == b->edges().end())
        throw std::runtime_error("Edge was not found");

    } else {
      edge = a->edges.size() < b->edges().size() ? a->edges().find(Edge(b)) : b->edges().find(Edge(a));
      if (edge == a->edges().end() || edge == b->edges().end())
        throw std::runtime_error("Edge was not found");

    }

    return edge->get_tag();
  }

  bool set_tag_edge(const VertexTag& a, const VertexTag& b, const EdgeTag& newdata) {
    VertexItr ia = m_g.find(Vertex(a));
    VertexItr ib = m_g.find(Vertex(b));
    return set_tag_edge(ia, ib, newdata);
  }

  bool set_tag_edge(const VertexItr& a, const VertexItr& b, const EdgeTag& newdata) {
    if (a == m_g.end() || b == m_g.end())
      return false;

    if constexpr (type == DIRECTED) {
      EdgeItr edge = a->edges().find(Edge(b));
      if (edge == a->edges().end())
        return false;

      edge->set_tag(newdata);

    } else {
      EdgeItr edge1 = a->edges().find(Edge(b));
      EdgeItr edge2 = b->edges().find(Edge(a));
      if (edge1 == a->edges().end() && edge2 == b->edges().end())
        return false;

      edge1->set_tag(newdata);
      edge2->set_tag(newdata);
    }

    return true;
  }

  void print(std::ostream& os = std::cout) {
    for (VertexItr ii = m_g.begin(); ii != m_g.end(); ii = std::next(ii)) {
      os << "[" << *ii << "] => {";

      for (EdgeItr jj = ii->edges().begin(); jj != ii->edges().end(); jj = std::next(jj)) {
        os << (*jj);
        if (std::next(jj) != ii->edges().end())
          os << ", ";
      }

      os << "}" << std::endl;
    }
  }

  void visit_bfs(std::function<void (const Vertex&)> visit_func) {
    std::queue<VertexItr> queue;
    queue.push(m_g.begin());

    VertexItr tmp;
    while (!queue.empty()) {
      tmp = queue.front();
            queue.pop();

      visit_func(*tmp);
      tmp->mark();

      for (auto& e : tmp->edges()) 
        if (!e.vertex().marked()) {
          queue.push(e.vertex_itr());
          e.vertex().mark();
        }
    }

    reset_marks();
  }

  void visit_dfs(std::function<void (const Vertex&)> visit_func) {
    std::stack<VertexItr> stack;
    stack.push(m_g.begin());

    VertexItr tmp;
    while (!stack.empty()) {
      tmp = stack.top();
            stack.pop();

      visit_func(*tmp);
      tmp->mark();

      for (auto& e : tmp->edges()) 
        if (!e.vertex().marked()) {
          stack.push(e.vertex_itr());
          e.vertex().mark();
        }
    }

    reset_marks();
  }

  bool existing_way(const VertexTag& a, const VertexTag& b) {
    VertexItr ia = m_g.find(Vertex(a));
    VertexItr ib = m_g.find(Vertex(b));
    return existing_way(ia, ib);
  }

  bool existing_way(const VertexItr& a, const VertexItr& b) {
    if (a == m_g.end() || b == m_g.end())
      throw std::runtime_error("One (or both) of the given vertex doesn't exist");

    if constexpr (type == DIRECTED) {

    } else {
      
    }
  }

  EdgeSet dijkstra_from(const VertexTag& a) {
    static_assert(
      std::is_arithmetic<EdgeTag>::value  ||
      is_pseudo_scalar<EdgeTag>::value    ||
      is_fully_comparable<EdgeTag>::value ,
      "Dijkstra only works for arithmetic type or pseudoscalar (fully comparables) EdgeTags."
    );


    VertexItr origin = m_g.find(Vertex(a));
    if (origin == m_g.end()) throw std::runtime_error("Vertex wasn\'t found");

    EdgeSet distances(origin->edges());
    origin->mark();

    MinFiboHeap<EdgeItr,
      typename Edge::tagless,
      typename Edge::tagequal,
      typename Edge::tagminus> heap;

    for (auto ii = distances.begin(); ii != distances.end(); ++ii)
      heap.add(ii);
      

    Edge    min;
    EdgeItr from_min;
    EdgeItr tmp;

    std::size_t used = 1;

    while (used < m_g.size()) {
      try { min = get_min_dijkstra(heap); } 
      catch (const std::exception& e) { break; }

      min.vertex().mark(); ++used;

      for (VertexItr ii = m_g.begin(); ii != m_g.end(); ++ii) {
        if (ii->marked()) continue;

        from_min = min.vertex().edges().find(Edge(ii));

        if (from_min != min.vertex().edges().end()) {
          tmp = distances.find(Edge(ii));

          if (tmp != distances.end()) 
            tmp->set_tag(std::min(tmp->get_tag(), min.get_tag() + from_min->get_tag())); 
          else 
            distances.emplace(from_min->vertex_itr(), min.get_tag() + from_min->get_tag());
        } 
      }
    }

    reset_marks();
    return distances;
  }

  Graph<VertexTag, EdgeTag, UNDIRECTED> mst_kruskall() {
    static_assert(type == UNDIRECTED, "Graph needs to be aled::UNDIRECTED to obtain MST");

    MinFiboHeap<FullyEdge> heap;
    fill_kruskall_heap(heap);

    Graph<VertexTag, EdgeTag, UNDIRECTED> mst;

    FullyEdge min;
    while (mst.no_vertexes() < m_no_vertexes) {
      std::pop_heap(heap.begin(), heap.end());
      min = heap.getMin(); heap.removeTop();

      if (!mst.cycle_with(min)) {
        auto b = mst.add_vertex(min.beg());
        auto e = mst.add_vertex(min.end());
        mst.add_edge(b->first, e->first, min.get_tag());
      }
    }

    return mst;
  }

  Graph<VertexTag, EdgeTag, UNDIRECTED> mst_prim() {
    static_assert(type == UNDIRECTED, "Graph needs to be aled::UNDIRECTED to obtain MST");

    Graph<VertexTag, EdgeTag, UNDIRECTED> mst;

    return mst;
  }

private:

  bool cycle_with(const FullyEdge& e) {
    if (m_g.empty()) return false;

    // f for first, s for second
    Vertex f = e.beg();
    Vertex s = e.end();

    if constexpr (type == DIRECTED) {

    } else {

    }

    // TODO: End function
  }

  void fill_kruskall_heap(MinFiboHeap<FullyEdge>& heap) {
    static_assert(type == UNDIRECTED, "Graph needs to be aled::UNDIRECTED to obtain MST");

    for (VertexItr v = m_g.begin(); v != m_g.end(); ++v) {
      for (EdgeItr e = v->edges().begin(); e != v->edges().end(); ++e) {
        if (e->vertex().marked()) continue;
        heap.add(FullyEdge(v, e->vertex_itr(), e->get_tag()));
      } 

      v->mark();
    }

    reset_marks();
  }

  Edge get_min_dijkstra(const EdgeSet& d) {
    MinFiboHeap<Edge, 
      typename Edge::tagless, 
      typename Edge::tagequal, 
      typename Edge::tagminus> heap;

    for (auto ii = d.begin(); ii != d.end(); ++ii)
      heap.add(*ii);

    Edge temp;
    while (!heap.empty()) {
      temp = heap.getMin();
             heap.removeTop();
      if (!temp.vertex().marked()) return temp;
    }

    throw std::logic_error("No more ways");
  }

  Edge get_min_dijkstra(MinFiboHeap<EdgeItr, typename Edge::tagless, typename Edge::tagequal, typename Edge::tagminus>& heap) {
    if (heap.empty()) throw std::logic_error("No more ways");

    Edge temp;
    temp = heap.getMin();
           heap.removeTop();

    return temp;
  }

  void reset_marks() {
    for (VertexItr ii = m_g.begin(); ii != m_g.end(); ++ii)
      ii->unmark();
  }

};

}

#endif
