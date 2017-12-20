cmake_minimum_required(VERSION 3.7)
project(qaedlib CXX)

set(CMAKE_CXX_STANDARD 17)
set(TEST_SRC_DIR ${PROJECT_SOURCE_DIR}/test)
set(TEST_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

find_package(PkgConfig REQUIRED)
pkg_search_module(GV REQUIRED libgvc)
pkg_search_module(X11 REQUIRED x11)

include_directories(include)
include_directories(${GV_INCLUDE_DIRS})

add_executable(avl_tree       ${TEST_SRC_DIR}/AVLTreeTest.cpp)
add_executable(array_heap     ${TEST_SRC_DIR}/ArrayHeapTest.cpp)
add_executable(binomial_heap  ${TEST_SRC_DIR}/BinomialHeapTest.cpp)
add_executable(fibonacci_heap ${TEST_SRC_DIR}/FibonacciHeapTest.cpp)
add_executable(redblack_tree  ${TEST_SRC_DIR}/RedBlackTreeTest.cpp)
add_executable(sbtree         ${TEST_SRC_DIR}/SimpleBinaryTreeTest.cpp)
add_executable(simple_list    ${TEST_SRC_DIR}/SimpleListTest.cpp)
add_executable(double_list    ${TEST_SRC_DIR}/DoubleListTest.cpp)
add_executable(stack          ${TEST_SRC_DIR}/StackTest.cpp)
add_executable(sparse_matrix  ${TEST_SRC_DIR}/SparseMatrixTest.cpp)
add_executable(cimg_spmatrix  ${TEST_SRC_DIR}/SparseMatrixTestCImg.cpp)
add_executable(graph          ${TEST_SRC_DIR}/GraphTest.cpp)
add_executable(hash_table     ${TEST_SRC_DIR}/HashTableTest.cpp)
add_executable(gv_tools       ${TEST_SRC_DIR}/GVToolsTest.cpp)

set_target_properties(
  avl_tree
  double_list
  array_heap
  binomial_heap
  fibonacci_heap
  redblack_tree
  sbtree
  simple_list
  stack
  gv_tools
  sparse_matrix
  graph
  hash_table

  PROPERTIES

  RUNTIME_OUTPUT_DIRECTORY  "${TEST_BIN_DIR}"
  LINK_LIBRARIES            "${GV_LIBRARIES}"
  COMPILE_FLAGS             "${GV_CFLAGS_OTHER}"
)

set_target_properties(
  cimg_spmatrix

  PROPERTIES

  RUNTIME_OUTPUT_DIRECTORY "${TEST_BIN_DIR}"
  LINK_LIBRARIES           "${X11_LIBRARIES}"
)

target_link_libraries(cimg_spmatrix pthread)
