#ifndef ALED_SPARSE_MATRIX
#define ALED_SPARSE_MATRIX

#include <map>
#include <tuple>
#include <fstream>
#include <iostream>

// TODO:
// - Add swap rows, cols
// - Remove row and col
// - Remove an element in pos(i,j)

namespace aled {

template <class Type>
class Matrix {
public:
  using Size_t = unsigned long;
  using Point  = std::tuple<Size_t, Size_t>;

private:
  static const int ROW = 0;
  static const int COL = 1;

  std::tuple<Size_t, Size_t> makePoint(const Size_t& x, const Size_t& y) { return std::make_tuple(x, y); }

  struct Node {

    Type  data;
    Point position;
    Node* next_col;
    Node* next_row;

    Node(Type&& data, const Point& position)      : data(data), position(position) {}
    Node(const Type& data, const Point& position) : data(data), position(position) {}

    Node(Type&& data, const Size_t& x, const Size_t& y)      : data(data), position(std::make_tuple(x,y)) {}
    Node(const Type& data, const Size_t& x, const Size_t& y) : data(data), position(std::make_tuple(x,y)) {}
  };

  Size_t m_no_rows;
  Size_t m_no_cols;

  Node** m_rows;
  Node** m_cols;

public:
  Matrix(const Size_t& rows, const Size_t& cols): m_no_rows(rows), m_no_cols(cols) {
    m_rows = new Node*[rows];
    m_cols = new Node*[cols];

    for (Size_t ii = 0; ii < rows; ++ii)
      m_rows[ii] = 0;

    for (Size_t ii = 0; ii < cols; ++ii)
      m_cols[ii] = 0;
  }

 ~Matrix() {}

  bool add(const Type& data, const Point& pos) {
    Node** r;
    Node** c;

    if (find_by_row(pos, c)) return false;
    find_by_col(pos, r);

    Node* tmp = new Node(data, pos);
    tmp->next_row = *r;
    tmp->next_col = *c;
    (*r) = (*c) = tmp;

    return true;
  }

  bool add(const Type& data, const Size_t& row, const Size_t& col) {
    return add(data, makePoint(row, col));
  }

  bool remove(const Point& pos) {
    Node** r;
    Node** c;

    if (!find_by_row(pos, c)) return false;
    find_by_col(pos, r);

    Node* tmp = *r;
    (*r) = tmp->next_row;
    (*c) = tmp->next_col;
    delete tmp;

    return true;
  }

  bool remove(const Size_t& row, const Size_t& col) {
    return remove(makePoint(row, col));
  }

  bool find(const Point& pos) {
    Node** tmp;
    return find_by_col(pos, tmp);
  }

  bool find(const Size_t& row, const Size_t& col) {
    return find(makePoint(row, col));
  }

  Type& get(const Point& pos) {
    Node** tmp;

    if (!find_by_col(pos, tmp))
      throw std::runtime_error("Cannot find element at pos " + 
          std::to_string(std::get<COL>(pos)) +
          std::to_string(std::get<ROW>(pos)));

    return (*tmp)->data;
  }

  Type& get(const Size_t& row, const Size_t& col) {
    return get(makePoint(row, col));
  }

  void print(std::ostream& os = std::cout) {
    Node** tmp;

    for (Size_t row = 0; row < m_no_rows; ++row) {
      for (Size_t col = 0; col < m_no_cols; ++col) {
        if (find_by_col(makePoint(row, col), tmp))
          os << (*tmp)->data << " ";
        else
          os << "0 ";
      }
      os << std::endl;
    }
  }

  void printLn(std::ostream& os = std::cout) {
    print(os); os << std::endl;
  }

private:

  bool find_by_row(const Point& pos, Node**& n) {
    n = &(m_rows[std::get<ROW>(pos)]);

    while (*n) {
      if (std::get<COL>((*n)->position) == std::get<COL>(pos)) return true;
      if (std::get<COL>((*n)->position)  > std::get<COL>(pos)) return false;

      n = &((*n)->next_col);
    }

    return false;
  }

  bool find_by_col(const Point& pos, Node**& n) {
    n = &(m_rows[std::get<COL>(pos)]);

    while (*n) {
      if (std::get<ROW>((*n)->position) == std::get<ROW>(pos)) return true;
      if (std::get<ROW>((*n)->position)  > std::get<ROW>(pos)) return false;

      n = &((*n)->next_row);
    }

    return false;
  }

};


template <class Type, Type def, class SizeT = unsigned long>
class FastMatrix {
public:
  using Size_t = SizeT;
  using Point  = std::tuple<Size_t, Size_t>;

private:
  static const int ROW = 0;
  static const int COL = 1;

  std::tuple<Size_t, Size_t> makePoint(const Size_t& x, const Size_t& y) { return std::make_tuple(x, y); }

  using RowIterator = typename std::map<Size_t, std::map<Size_t, Type>>::iterator;
  using ColIterator = typename std::map<Size_t, Type>::iterator;

  std::map<Size_t, std::map<Size_t, Type>> m_matrix;
  Size_t m_no_rows;
  Size_t m_no_cols;


public:
  FastMatrix(const Size_t& r, const Size_t& c): m_no_rows(r), m_no_cols(c) {}

  void add(const Type& data, const Point& pos) {
    Size_t row = std::get<ROW>(pos);
    Size_t col = std::get<COL>(pos);

    if (row >= m_no_rows) return;
    if (col >= m_no_cols) return;

    RowIterator rit;
    ColIterator cit;

    if (find(pos, rit, cit)) return;

    if (rit == m_matrix.end()) {
      std::map<Size_t, Type> tmp;
      tmp.emplace(col, data);
      m_matrix.emplace(row, tmp);
    } else {
      rit->second.emplace(col, data);
    }
  }

  void add(const Type& data, const Size_t& row, const Size_t& col) {
    add(data, makePoint(row, col));
  }

  bool find(const Point& pos) {
    Size_t row = std::get<ROW>(pos);
    Size_t col = std::get<COL>(pos);

    if (row >= m_no_rows) return false;
    if (col >= m_no_cols) return false;

    RowIterator rit;
    ColIterator cit;

    return find(pos, rit, cit);
  }

  bool find(const Size_t& row, const Size_t& col) {
    return find(makePoint(row, col));
  }

  Type& get(const Point& pos) {
    Size_t row = std::get<ROW>(pos);
    Size_t col = std::get<COL>(pos);

    if (row >= m_no_rows) throw std::out_of_range("rows out of range");
    if (col >= m_no_cols) throw std::out_of_range("cols out of range");

    RowIterator rit;
    ColIterator cit;

    if (!find(pos, rit, cit))
      throw std::runtime_error("Cannot find element at pos " + 
          std::to_string(std::get<COL>(pos)) +
          std::to_string(std::get<ROW>(pos)));

    return cit->second;
  }

  Type& get(const Size_t& row, const Size_t& col) {
    return get(makePoint(row, col));
  }

  void remove(const Point& pos) {
    Size_t row = std::get<ROW>(pos);
    Size_t col = std::get<COL>(pos);

    if (row >= m_no_rows) return;
    if (col >= m_no_cols) return;

    RowIterator rit;
    ColIterator cit;

    if (!find(pos, rit, cit))
      return;

    rit->second.erase(cit);
  }

  void remove(const Size_t& row, const Size_t& col) {
    remove(makePoint(row, col));
  }

  void print(std::ostream& os = std::cout) {
    RowIterator rit;
    ColIterator cit;

    for (Size_t ii = 0; ii < m_no_rows; ++ii) {
      for (Size_t jj = 0; jj < m_no_cols; ++jj) {
        if (find(makePoint(ii,jj), rit, cit))
          os << cit->second << " ";
        else
          os << def << " ";
      }

      os << std::endl;
    }
  }

  void printLn(std::ostream& os = std::cout) {
    print(os); os << std::endl;
  }

  void write(const std::string filename, bool binary = true) {
    RowIterator rit;
    ColIterator cit;

    if (binary) {
      std::fstream out(filename, std::ios::out | std::ios::binary);

      char* tmp;

      tmp = (char*) &m_no_rows;
      out.write(tmp, sizeof(Size_t));
      tmp = (char*) &m_no_cols;
      out.write(tmp, sizeof(Size_t));
      for (Size_t ii = 0; ii< m_no_rows; ++ii) {
        for (Size_t jj = 0; jj < m_no_cols; ++jj) {
          if (find(makePoint(ii,jj), rit, cit)) {
            tmp = (char*) &(cit->second);
            out.write(tmp, sizeof(Type));
            tmp = (char*) &ii;
            out.write(tmp, sizeof(Size_t));
            tmp = (char*) &jj;
            out.write(tmp, sizeof(Size_t));
          }
        }
      }

    } else {
      std::fstream out(filename, std::ios::out);

      out << m_no_rows << ' ' << m_no_cols << ' ';
      for (Size_t ii = 0; ii < m_no_rows; ++ii) {
        for (Size_t jj = 0; jj < m_no_cols; ++jj)
          if (find(makePoint(ii,jj), rit, cit))
            out << cit->second << ' ' << ii << ' ' << jj << ' ';
      }
    }
  }

  void read(const std::string filename, bool binary = true) {
    if (binary) {
      std::fstream in(filename, std::ios::in | std::ios::binary);

      char* tmp_sizet = new char[sizeof(Size_t)];
      char* tmp_type  = new char[sizeof(Type)];

      in.read(tmp_sizet, sizeof(Size_t));
      m_no_rows = *((Size_t*) tmp_sizet);

      in.read(tmp_sizet, sizeof(Size_t));
      m_no_cols = *((Size_t*) tmp_sizet);

      Size_t row, col;
      Type   data;
      while (in) {
        in.read(tmp_type, sizeof(Type));
        data = *((Type*) tmp_type);
        in.read(tmp_sizet, sizeof(Size_t));
        row = *((Size_t*) tmp_sizet);
        in.read(tmp_sizet, sizeof(Size_t));
        col = *((Size_t*) tmp_sizet);
        add(data, row, col);
      }

      delete[] tmp_sizet;
      delete[] tmp_type;

    } else {
      std::fstream in(filename, std::ios::in);

      in >> m_no_rows >> m_no_cols;

      Size_t row, col;
      Type   data;
      while (in) {
        in >> data >> row >> col;
        add(data, row, col);
      }
    }
  }

  Type get_default() const { return def; }

  const Size_t& noRows() const { return m_no_rows; }
  const Size_t& noCols() const { return m_no_cols; }

private:

  bool find(const Point& pos, RowIterator& rit, ColIterator& cit) {
    Size_t row = std::get<ROW>(pos);
    Size_t col = std::get<COL>(pos);

    rit = m_matrix.find(row);
    if (rit == m_matrix.end())
      return false;

    cit = rit->second.find(col);
    if (cit == rit->second.end())
      return false;

    return true;
  }
};

}

#endif
