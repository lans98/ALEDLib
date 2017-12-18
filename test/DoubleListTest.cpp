#include <iostream>
#include <memory>
#include "lists/double_list.hpp"

using namespace std;

int main() {
  auto list = make_unique<aled::DoubleList<int>>();
  list->add(3);
  list->add(5);
  list->add(6);
  list->add(4);
  list->add(1);
  list->add(2);

  list->printLn(cout);

  list->rmv(6);
  list->rmv(2);
  list->rmv(3);

  list->printLn(cout);

  cout << "Iterators:\n";
  aled::DoubleList<int>::Iterator it = list->begin();
  for (; it != list->end(); ++it) 
    cout << *it << ' ';
  cout << endl;

  it = list->last();
  for (; it != list->end(); --it)
    cout << *it << ' ';
  cout << endl;

  return 0;
}
