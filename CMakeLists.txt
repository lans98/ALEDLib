cmake_minimum_required(VERSION 3.7)
project(ALEDLib CXX)

set(CMAKE_CXX_STANDARD 17)
set(TEST_SRC_DIR ${PROJECT_SOURCE_DIR}/test)
set(TEST_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

find_package(PkgConfig REQUIRED)
pkg_search_module(GV REQUIRED libgvc)
pkg_search_module(X11 REQUIRED x11)

include_directories(inc)
include_directories(${GV_INCLUDE_DIRS})

add_executable(avl
  ${TEST_SRC_DIR}/avl_tree_test/avl_tree_test.cc)

add_executable(double_list
  ${TEST_SRC_DIR}/double_list_test/double_list_test.cc)

add_executable(maxarr_heap
  ${TEST_SRC_DIR}/max_arr_heap_test/max_arr_heap_test.cc)

add_executable(minarr_heap
  ${TEST_SRC_DIR}/min_arr_heap_test/min_arr_heap_test.cc)

add_executable(maxbinom_heap
  ${TEST_SRC_DIR}/max_binom_heap_test/max_binom_heap_test.cc)

add_executable(minbinom_heap
  ${TEST_SRC_DIR}/min_binom_heap_test/min_binom_heap_test.cc)

add_executable(maxfibo_heap
  ${TEST_SRC_DIR}/max_fibo_heap_test/max_fibo_heap_test.cc)

add_executable(minfibo_heap
  ${TEST_SRC_DIR}/min_fibo_heap_test/min_fibo_heap_test.cc)

add_executable(redblack_tree
  ${TEST_SRC_DIR}/redblack_tree_test/redblack_tree_test.cc)

add_executable(sbtree
  ${TEST_SRC_DIR}/simple_bin_tree_test/simple_bin_tree_test.cc)

add_executable(simple_list
  ${TEST_SRC_DIR}/simple_list_test/simple_list_test.cc)

add_executable(stack
  ${TEST_SRC_DIR}/stack_test/stack_test.cc)

add_executable(gv_tools
  ${TEST_SRC_DIR}/graphviz_tools_test/graphviz_tools_test.cc)

add_executable(sparse_matrix
  ${TEST_SRC_DIR}/sparse_matrix_test/sparse_matrix_test.cc)

add_executable(img_spmatrix
  ${TEST_SRC_DIR}/sparse_cimg/sparse_cimg.cc)

add_executable(graph_test
  ${TEST_SRC_DIR}/graph_test/graph_test.cc)

add_executable(hash_test
  ${TEST_SRC_DIR}/hash_test/hash_test.cc)

set_target_properties(
  avl
  double_list
  maxarr_heap
  minarr_heap
  maxbinom_heap
  minbinom_heap
  maxfibo_heap
  minfibo_heap
  redblack_tree
  sbtree
  simple_list
  stack
  gv_tools
  sparse_matrix
  graph_test
  hash_test

  PROPERTIES

  RUNTIME_OUTPUT_DIRECTORY  "${TEST_BIN_DIR}"
  LINK_LIBRARIES            "${GV_LIBRARIES}"
  COMPILE_FLAGS             "${GV_CFLAGS_OTHER}"
)

set_target_properties(
  img_spmatrix

  PROPERTIES

  RUNTIME_OUTPUT_DIRECTORY "${TEST_BIN_DIR}"
  LINK_LIBRARIES           "${X11_LIBRARIES}"
)

target_link_libraries(img_spmatrix pthread)
