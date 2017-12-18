#include <iostream>
#include <memory>

#include "SimpleList.hpp"

using namespace std;

int main() {

  auto list = make_unique<qaed::SimpleList<int>>();
  
  list->add(3);
  list->add(4);
  list->add(2);
  list->add(7);
  list->add(1);
  list->add(8);

  list->printLn(cout);

  list->rmv(3);
  list->rmv(8);
  list->rmv(2);
  
  list->printLn(cout);

  cout << "Iterators:\n";
  qaed::SimpleList<int>::Iterator it = list->begin();
  for (; it != list->end(); ++it)
    cout << *it << ' ';
  cout << endl;

  return 0;
}
