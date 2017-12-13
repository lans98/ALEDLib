#include "redblack_tree/redblack_tree.h"

int main() {
  aled::RedBlackTree<int> rb;
  rb.add(5);
  rb.add(15);
  rb.add(20);

  rb.printBFS();
  return 0;
}
