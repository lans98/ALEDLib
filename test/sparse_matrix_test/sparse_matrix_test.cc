#include "sparse_matrix/sparse_matrix.h"
#include <memory>

int main() {
  auto sp_matrix = std::make_unique<aled::Matrix<int>>(10,10);

  std::cout << "Sparse Matrix" << std::endl;

  for (int i = 0; i < 10; ++i) 
    sp_matrix->add(1, i, i);
    
  sp_matrix->printLn();
  sp_matrix->remove(4,4);
  sp_matrix->printLn();
  sp_matrix->add(2, 5, 2);
  sp_matrix->printLn();

  auto fast_spmatrix = std::make_unique<aled::FastMatrix<int, 0>>(10, 10);

  std::cout << "Fast Matrix:" << std::endl;
  for (int i = 0; i < 10; ++i)
    fast_spmatrix->add(1, i, i);

  fast_spmatrix->printLn();
  fast_spmatrix->remove(4,4);
  fast_spmatrix->printLn();
  fast_spmatrix->add(2, 5, 2);
  fast_spmatrix->get(5,5) = 2;
  fast_spmatrix->add(1, 0, 2);
  fast_spmatrix->add(1, 0, 5);
  fast_spmatrix->printLn();

  fast_spmatrix->write("out");

  auto fast_spmatrix2 = std::make_unique<aled::FastMatrix<int, 0>>(0,0);
  fast_spmatrix2->read("out");
  std::cout << "Fast Matrix 2:" << std::endl;
  fast_spmatrix2->printLn();

  return 0;
}
