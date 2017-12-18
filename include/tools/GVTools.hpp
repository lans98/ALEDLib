#ifndef ALED_GVTOOLS_H
#define ALED_GVTOOLS_H 

#include <gvc.h>
#include <string>
#include <cstdlib>
#include <cstring>

#include <memory>

namespace aled {

class GVTool {
private:
  Agraph_t *m_graph;
  GVC_t    *m_gvc;

  void str_to_pointer_char(std::unique_ptr<char>& chrp, const std::string& str) {
    chrp = std::make_unique<char>(str.length() + 1);
    std::strcpy(chrp.get(), str.c_str());
  }

  void assert_graph_not_null() {
    if (!m_graph) throw std::runtime_error("Not existing graph");
  }

public:
  GVTool(): m_graph(nullptr), m_gvc(gvContext()) {
    if (!m_gvc) throw std::runtime_error("Failed to allocate GraphViz context");
  }
 
 ~GVTool() { 
    gvFreeLayout(m_gvc, m_graph);
    gvFreeContext(m_gvc); 
  }

  void clearGraph() { agclose(m_graph); }

  Agraph_t* createGraph(const std::string& name, Agdesc_t desc, Agdisc_t* disc = 0)  { 
    std::unique_ptr<char> name_chr;
    str_to_pointer_char(name_chr, name);

    m_graph = agopen(name_chr.get(), desc, disc);
    return m_graph; 
  }

  Agraph_t* resetGraph(const std::string& name, Agdesc_t desc, Agdisc_t* disc = 0) {
    clearGraph();
    return createGraph(name, desc, disc);
  }

  Agraph_t* mainGraph() { return m_graph; }

  Agraph_t* subgraph(const std::string& name, Agraph_t* g = 0, int createflag = 1) { 
    if (!g) {
      assert_graph_not_null();
      g = m_graph;
    }

    std::unique_ptr<char> name_chr;
    str_to_pointer_char(name_chr, name);

    return agsubg(g, name_chr.get(), createflag); 
  }

  Agnode_t* node(const std::string& name,Agraph_t* g = 0, int createflag = 1) { 
    if (!g) {
      assert_graph_not_null();
      g = m_graph;
    }

    std::unique_ptr<char> name_chr;
    str_to_pointer_char(name_chr, name);

    return agnode(g, name_chr.get(), createflag); 
  }

  Agedge_t* edge(Agnode_t* head, Agnode_t* tail, Agraph_t* g = 0, const std::string& name = "", int createflag = 1) { 
    if (!g) {
      assert_graph_not_null();
      g = m_graph;
    }

    std::unique_ptr<char> name_chr;
    str_to_pointer_char(name_chr, name);

    return agedge(m_graph, head, tail, name_chr.get(), createflag); 
  }

  Agedge_t* edge(const std::string& head, const std::string& tail, Agraph_t* g = 0, const std::string& name = "", int createflag = 1) { 
    return edge(node(head), node(tail), g, name, createflag); 
  }

  Agsym_t* defAttr(int kind, const std::string& name, const std::string& value) { 
    assert_graph_not_null();

    std::unique_ptr<char> name_chr, value_chr;
    str_to_pointer_char(name_chr, name);
    str_to_pointer_char(value_chr, value);

    return agattr(m_graph, kind, name_chr.get(), value_chr.get()); 
  }

  std::string getAttr(Agraph_t* graph, const std::string& attr) { 
    assert_graph_not_null();

    std::unique_ptr<char> attr_chr;
    str_to_pointer_char(attr_chr, attr);

    attr_chr.reset(agget(graph, attr_chr.get()));
    return std::string(attr_chr.get());
  }

  int setAttr(Agraph_t* graph, const std::string& attr, const std::string& value) { 
    assert_graph_not_null();

    std::unique_ptr<char> attr_chr, value_chr;
    str_to_pointer_char(attr_chr, attr);
    str_to_pointer_char(value_chr, value);

    return agset(graph, attr_chr.get(), value_chr.get()); 
  }

  std::string getAttr(Agnode_t* node, const std::string& attr) { 
    assert_graph_not_null();

    std::unique_ptr<char> attr_chr;
    str_to_pointer_char(attr_chr, attr);

    attr_chr.reset(agget(node, attr_chr.get()));
    return std::string(attr_chr.get());
  }

  int setAttr(Agnode_t* node, const std::string& attr, const std::string& value) { 
    assert_graph_not_null();

    std::unique_ptr<char> attr_chr, value_chr;
    str_to_pointer_char(attr_chr, attr);
    str_to_pointer_char(value_chr, value);

    return agset(node, attr_chr.get(), value_chr.get()); 
  }

  std::string getAttr(Agedge_t* edge, const std::string& attr) { 
    assert_graph_not_null();

    std::unique_ptr<char> attr_chr;
    str_to_pointer_char(attr_chr, attr);

    attr_chr.reset(agget(edge, attr_chr.get()));
    return std::string(attr_chr.get());
  }

  int setAttr(Agedge_t* edge, const std::string& attr, const std::string& value) { 
    assert_graph_not_null();

    std::unique_ptr<char> attr_chr, value_chr;
    str_to_pointer_char(attr_chr, attr);
    str_to_pointer_char(value_chr, value);

    return agset(edge, attr_chr.get(), value_chr.get()); 
  }

  const std::string& addHTMLString(const std::string& str, Agraph_t* g = 0) {
    if (!g) {
      assert_graph_not_null();
      g = m_graph;
    }

    std::unique_ptr<char> str_chr;
    str_to_pointer_char(str_chr, str);
    
    agstrdup_html(g, str_chr.get());
    return str;
  }

  int layout(const char* engine) { 
    assert_graph_not_null();

    return  gvLayout(m_gvc, m_graph, engine); 
  }

  int freeLayout() {
    assert_graph_not_null();

    return gvFreeLayout(m_gvc, m_graph);
  }

  int renderToFile(const char* format, const char* fname) {
    assert_graph_not_null();

    return gvRenderFilename(m_gvc, m_graph, format, fname);
  }

  void xdgOpenFile(const char* fname) {
    if (!fname || !std::strcmp(fname, "")) throw std::runtime_error("No valid file");
    std::system((std::string("xdg-open ") + std::string(fname)).c_str());
  }

};

}

#endif
