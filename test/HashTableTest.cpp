#include "HashTable.hpp"
#include <memory>

int main() {
  auto hashing = [](const int& a){ return (a << 2) >> 1; };

  auto hash = std::make_unique<qaed::Hash<int, 7>>(hashing);

  hash->add(2312);
  hash->add(123);
  hash->add(1246);
  hash->add(231664);
  hash->add(784);

  std::cout << "Hash\n";
  hash->print();

  std::cout << "Remove 123\n";
  std::cout << std::boolalpha << hash->remove(123) << '\n';
  hash->print();

  std::cout << "Updated 1246 to 1249\n";
  std::cout << std::boolalpha << hash->update(1246, 1249) << '\n';
  hash->print();

  return 0;
}
