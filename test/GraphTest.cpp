#include <memory>
#include <chrono>

#include "Graph.hpp"

class A {};

class B {
public:
  bool operator==(const B& b) const ; // Success
//void operator==(const B& b) const ; // Fail
  bool operator!=(const B& b) const;
  bool operator<(const B& b) const;
  bool operator<=(const B& b) const;
  bool operator>(const B& b) const;
  bool operator>=(const B& b) const;
};

int main() {
  aled::Graph<int, int, aled::DIRECTED> g1;

  g1.add_vertex(3);
  g1.add_vertex(2);
  g1.add_vertex(1);
  g1.add_vertex(8);
  g1.add_vertex(9);

  g1.add_edge(3, 2, 5);
  g1.add_edge(9, 2, 3);
  g1.add_edge(8, 3, 1);
  g1.add_edge(2, 9, 7);

  std::cout << "Graph g1:\n";
  g1.print();

  std::cout << "Added edge 3 ->(6) 9\n";
  g1.add_edge(3, 9, 6);
  g1.print();

  std::cout << "Removed edge 3->2 \n";
  g1.remove_edge(3, 2);
  g1.print();

  std::cout << "Added some edges to continue testing\n";
  g1.add_edge(3, 8, 2);
  g1.add_edge(1, 8, 1);
  g1.add_edge(2, 8, 3);
  g1.add_edge(9, 8, 5);
  g1.print();

  std::cout << "Calculating dijkstra_from 3\n";
  auto v = g1.dijkstra_from(3);
  for (auto& x : v)
    std::cout << x << std::endl;

  std::cout << "Removed edges containing vertex 8\n";
  g1.remove_edges_with(8);
  g1.print();

  std::cout << "Remove vertex 9\n";
  g1.remove_vertex(9);
  g1.print();

  //aled::Graph<A, A> g4;
  //Static assert fails for type A that isn't comparable

  aled::Graph<std::string, int, aled::DIRECTED> g2;
  g2.add_vertex("hola");
  g2.add_vertex("mundo");

  std::cout << "\nGraph g2:\n";
  g2.print();

  std::cout << "Added edge hola -(2)> mundo\n";
  g2.add_edge("hola", "mundo", 2);
  g2.print();

  aled::Graph<B, B, aled::DIRECTED> g3;
  // Fails because graph needs to be undirected
  // auto mst_g3 = g3.mst_kruskall();

  aled::Graph<char, int, aled::UNDIRECTED> g4;
  g4.add_vertex('a');
  g4.add_vertex('b');
  g4.add_vertex('c');
  g4.add_vertex('d');
  g4.add_vertex('e');
  g4.add_vertex('f');

  g4.add_edge('a', 'b', 1);
  g4.add_edge('a', 'd', 5);
  g4.add_edge('a', 'c', 4);
  g4.add_edge('b', 'd', 2);
  g4.add_edge('b', 'c', 1);
  g4.add_edge('c', 'd', 1);
  g4.add_edge('c', 'e', 5);
  g4.add_edge('c', 'f', 2);
  g4.add_edge('d', 'f', 7);
  g4.add_edge('d', 'e', 1);
  g4.add_edge('f', 'e', 6);

  std::cout << "Graph g4:\n";
  g4.print();
  std::cout << "BFS g4:\n";
  g4.visit_bfs([](auto v){ std::cout << v << ' '; });
  std::cout << "\nDFS g4:\n";
  g4.visit_dfs([](auto v){ std::cout << v << ' '; });

  std::cout << "\nDijkstra from 'a' in g4:\n";
  std::clock_t start = clock();
  auto d = g4.dijkstra_from('a');
  std::clock_t end  = clock();
  std::cout << "time for dijkstra:" << 1000.0 * (end - start) / CLOCKS_PER_SEC << std::endl;
  for (auto& x : d)
    std::cout << x << std::endl;

  std::cout << "MST kruskall for g4:\n";
  start = clock();
  auto g4_mst = g4.mst_kruskal();
  end = clock();
  g4_mst.print();
  std::cout << "time for kruskall mst: " << 1000.0 * (end - start) / CLOCKS_PER_SEC << std::endl;

  std::cout << "MST prim for g4:\n";
  start = clock();
  auto g4_mst2 = g4.mst_prim();
  end = clock();
  g4_mst2.print();
  std::cout << "time for prim mst: " << 1000.0 * (end - start) / CLOCKS_PER_SEC << std::endl;

  
  aled::Graph<char, int, aled::UNDIRECTED> g5;
  g5.add_vertex('x');
  g5.add_vertex('y');
  g5.add_vertex('z');

  g5.add_edge('x', 'y', 2);
  g5.add_edge('y', 'z', 3);

  auto g5_mst = g5.mst_kruskal();
  std::cout << "MST for g5:\n";
  g5_mst.print();
  
  return 0;
}
