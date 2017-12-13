#include "avl_tree/avl_tree.h"

int main() {
  aled::AVL<int> avl;

  avl.add(4);
  avl.add(3);
  avl.add(5);
  avl.add(6);
  avl.add(1);
  avl.add(9);
  avl.add(8);
  avl.add(7);
  avl.add(2);
  avl.add(10);
  avl.add(15);
  avl.add(12);
  avl.add(17);
  avl.add(16);
  avl.add(19);
  avl.add(27);
  avl.add(25);
  avl.add(24);
  avl.add(24);
  avl.add(29);
  avl.add(28);
  avl.add(31);
  avl.add(34);
  avl.add(37);
  avl.add(47);
  avl.add(18);
  avl.add(-98);
  avl.add(56);
  avl.add(34);
  avl.add(51);
  avl.add(12);
  avl.add(186);
  avl.add(114);
  avl.add(111);
  avl.add(110);
  avl.add(117);
  avl.add(151);
  avl.add(112);
  avl.add(151);
  avl.add(911);
  avl.add(411);
  avl.add(311);
  avl.add(211);
  avl.add(2011);
  avl.add(5511);
  avl.add(-965);

  avl.printLn();

  return 0;
}
