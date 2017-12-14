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
#include "graphviz_tools/gvtools.h"

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

    struct taggreater {
      bool operator()(const Edge& a, const Edge& b) { return a.m_data > b.m_data; }
      bool operator()(const EdgeItr& a, const EdgeItr& b) { return a->get_tag() > b->get_tag(); }
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

    bool operator==(const FullyEdge& e) const { return m_data == e.m_data; }
    bool operator!=(const FullyEdge& e) const { return m_data != e.m_data; }
    bool operator<(const FullyEdge& e)  const { return m_data <  e.m_data; }
    bool operator>(const FullyEdge& e)  const { return m_data >  e.m_data; }
    bool operator<=(const FullyEdge& e) const { return m_data <= e.m_data; }
    bool operator>=(const FullyEdge& e) const { return m_data >= e.m_data; }

    FullyEdge operator-(const FullyEdge& e) { FullyEdge t(*this); t.m_data -= e.m_data; return t; }

    const Vertex& beg() const { return *m_beg; }
    const Vertex& end() const { return *m_end; }

    const VertexItr& beg_itr() const { return m_beg; }
    const VertexItr& end_itr() const { return m_end; }

    const EdgeTag& get_tag() const { return m_data; }
    void set_tag(const EdgeTag& d) { return m_data = d; }

    operator Edge() { return Edge(m_end, m_data); }

    friend std::ostream& operator<<(std::ostream& os, const FullyEdge& e) {
      os << "(" << *e.m_beg  << ", " << e.m_data << ", " << *e.m_end << ")";
      return os;
    }

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

  auto get_vertex_itr(const VertexTag& a) { return m_g.find(Vertex(a)); }
  auto get_vertex(const VertexTag& a) { return *get_vertex_itr(a);}

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


  void visit_bfs(const std::function<void (const Vertex&)>& visit_func, VertexItr beg) {
    std::queue<VertexItr> queue;
    queue.push(beg);

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

  void visit_bfs(const std::function<void (const Vertex&)>& visit_func) { visit_bfs(visit_func, m_g.begin()); }

  void visit_dfs(const std::function<void (const Vertex&)>& visit_func, VertexItr beg) {
    std::stack<VertexItr> stack;
    stack.push(beg);

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
  
  void visit_dfs(const std::function<void (const Vertex&)>& visit_func) { visit_dfs(visit_func, m_g.begin()); }

  bool existing_way(const VertexTag& a, const VertexTag& b) {
    VertexItr ia = m_g.find(Vertex(a));
    VertexItr ib = m_g.find(Vertex(b));
    return existing_way(ia, ib);
  }

  bool existing_way(const VertexItr& a, const VertexItr& b) {
    if (a == m_g.end() || b == m_g.end())
      throw std::runtime_error("One (or both) of the given vertex doesn't exist");

    auto eb = a->edges().find(Edge(b));
    if (eb != a->edges().end()) return true;

    try {
      visit_dfs([&b](auto& v){ if (v == *b) throw std::exception(); }, a);
    } catch (const std::exception& e) { return true; }
    
    return false;
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

    std::vector<EdgeItr> heap;
    heap.reserve(m_no_vertexes - 1);
    for (auto ii = distances.begin(); ii != distances.end(); ++ii)
      heap.push_back(ii);

    Edge    min_edge;
    EdgeItr from_min;
    EdgeItr tmp;

    std::size_t used = 1;

    while (used < m_g.size()) {
      try { min_edge = get_min_dijkstra(heap); } 
      catch (const std::exception& e) { break; }

      min_edge.vertex().mark(); 
      ++used;

      for (VertexItr ii = m_g.begin(); ii != m_g.end(); ++ii) {
        if (ii->marked()) continue;

        from_min = min_edge.vertex().edges().find(Edge(ii));

        if (from_min != min_edge.vertex().edges().end()) {
          tmp = distances.find(Edge(ii));

          if (tmp != distances.end()) {
            tmp->set_tag(std::min(tmp->get_tag(), min_edge.get_tag() + from_min->get_tag())); 
            // heap automatically updates because it saves iterators
          } else {
            auto r = distances.emplace(from_min->vertex_itr(), min_edge.get_tag() + from_min->get_tag());
            heap.push_back(std::get<0>(r));
            std::push_heap(heap.begin(), heap.end(), typename Edge::taggreater());
          }
        } 
      }
    }

    reset_marks();
    return distances;
  }

  Graph<VertexTag, EdgeTag, UNDIRECTED> mst_kruskall() {
    static_assert(type == UNDIRECTED, "Graph needs to be aled::UNDIRECTED to obtain MST");

    std::vector<FullyEdge> heap;
    fill_kruskall_heap(heap);

    Graph<VertexTag, EdgeTag, UNDIRECTED> mst;

    FullyEdge min_edge;
    while (mst.no_edges() + 1 < m_no_vertexes && !heap.empty()) {
      std::pop_heap(heap.begin(), heap.end(), std::greater<FullyEdge>());
      min_edge = heap.back(); heap.pop_back();

      if (!mst.cycle_with(min_edge)) {
        auto b = mst.add_vertex(min_edge.beg().get_data());
        auto e = mst.add_vertex(min_edge.end().get_data());
        mst.add_edge(std::get<0>(b), std::get<0>(e), min_edge.get_tag());
      } 
    }

    return mst;
  }

  Graph<VertexTag, EdgeTag, UNDIRECTED> mst_prim() {
    static_assert(type == UNDIRECTED, "Graph needs to be aled::UNDIRECTED to obtain MST");

    std::vector<FullyEdge> heap;
    heap.reserve(m_no_edges/2);

    auto beg = m_g.begin();
    for (auto ii = beg->edges().begin(); ii != beg->edges().end(); ++ii) {
      if (ii->vertex().marked()) continue;
      heap.push_back(FullyEdge(beg, ii->vertex_itr(), ii->get_tag()));
    }
    beg->mark();


    Graph<VertexTag, EdgeTag, UNDIRECTED> mst;

    FullyEdge min_edge;
    while (mst.no_edges() + 1 < m_no_vertexes && !heap.empty()) {
      std::pop_heap(heap.begin(), heap.end(), std::greater<FullyEdge>());
      min_edge = heap.back(); heap.pop_back();

      if (!mst.cycle_with(min_edge)) {
        auto b = mst.add_vertex(min_edge.beg().get_data());
        auto e = mst.add_vertex(min_edge.end().get_data());
        mst.add_edge(std::get<0>(b), std::get<0>(e), min_edge.get_tag());
      }

      fill_prim_heap(heap, min_edge.end_itr());
    }

    reset_marks();
    return mst;
  }

  std::vector<Graph<VertexTag, EdgeTag, type>> scc() {

  }

  void draw_it(const std::string& filename) {
    if (m_g.empty()) return;

  }

private:

  bool cycle_with(const FullyEdge& e) {
    if (m_g.empty()) return false;

    try {
      return existing_way(e.end().get_data(), e.beg().get_data());
    } catch (const std::exception& e) { return false; }
  }

  void fill_kruskall_heap(std::vector<FullyEdge>& heap) {
    static_assert(type == UNDIRECTED, "Graph needs to be aled::UNDIRECTED to obtain MST");

    heap.reserve(m_no_edges/2);
    for (VertexItr v = m_g.begin(); v != m_g.end(); ++v) {
      for (EdgeItr e = v->edges().begin(); e != v->edges().end(); ++e) {
        if (e->vertex().marked()) continue;
        heap.push_back(FullyEdge(v, e->vertex_itr(), e->get_tag()));
      }

      v->mark();
    }

    reset_marks();
    std::make_heap(heap.begin(), heap.end(), std::greater<FullyEdge>());
  }

  void fill_prim_heap(std::vector<FullyEdge>& heap, VertexItr v) {
    for (auto ii = v->edges().begin(); ii != v->edges().end(); ++ii) {
      if (ii->vertex().marked()) continue;
      heap.push_back(FullyEdge(v, ii->vertex_itr(), ii->get_tag()));
      std::push_heap(heap.begin(), heap.end(), std::greater<FullyEdge>());
    }
    v->mark();
  }


  Edge get_min_dijkstra(std::vector<EdgeItr>& h) {
    if (h.empty()) throw std::logic_error("No more ways");

    std::make_heap(h.begin(), h.end(), typename Edge::taggreater());
    
    EdgeItr temp;
    std::pop_heap(h.begin(), h.end(), typename Edge::taggreater());
    temp = h.back(); h.pop_back();
    return *temp;
  }

  void reset_marks() {
    for (VertexItr ii = m_g.begin(); ii != m_g.end(); ++ii)
      ii->unmark();
  }

};

}

#endif
