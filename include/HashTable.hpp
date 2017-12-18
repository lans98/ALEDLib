#ifndef ALED_HASH_H
#define ALED_HASH_H

#include <set>
#include <vector>
#include <iostream>
#include <functional>

#include "tools/Sfinae.hpp"

namespace aled {

/// Consider using Size with an prime number
/// remember that hash uses modulo, if we use
/// an prime number as modulo, chances for
/// collisions in the same place are lowest,
/// and said that collisions are equally
/// distributed
template <class Type, std::size_t Size>
class Hash {
private:
  using HashTable    = typename std::vector<std::set<Type>>;
  using HashFunction = typename std::function<std::size_t (const Type&)>;

  HashTable    mHash;
  HashFunction mHf;

public:
  Hash() : mHash(Size) {
    static_assert(
      std::is_arithmetic<Type>::value        ||
      std::is_enum<Type>::value              ||
      std::is_pointer<Type>::value           ||
      std::is_same<Type, std::string>::value ||
      is_comparable<Type>::value      ,
      "Type is not an arithmetic, enum, pointer type nor string nor comparable object."
    );
  }

  Hash(const HashFunction& f): mHash(Size), mHf(f) {
    static_assert(
      std::is_arithmetic<Type>::value        ||
      std::is_enum<Type>::value              ||
      std::is_pointer<Type>::value           ||
      std::is_same<Type, std::string>::value ||
      is_comparable<Type>::value      ,
      "Type is not an arithmetic, enum, pointer type nor string nor comparable object."
    );
  }

  /// Elements need to be comparable between them
  /// Operators ==, !=, >, < should be overloaded
  bool add(const Type& data) {
    if (!mHf) throw std::runtime_error("Hash function was not defined");

    auto result = mHash[mHf(data) % Size].insert(data);
    return std::get<1>(result);
  }

  bool remove(const Type& data) {
    if (!mHf) throw std::runtime_error("Hash function was not defined");

    auto& cont = mHash[mHf(data) % Size];
    auto  elem = cont.find(data);

    if (elem == cont.end())
      return false;

    cont.erase(elem);
    return true;
  }

  /// Note how this method seems to be useless
  /// but, it's powerful when using Type like
  /// Records, pairs, tuples, etc, any kind of
  /// object that only needs a primary key.
  /// Here you pass data, like an object only
  /// with the primary key you are searching for
  /// (if you overloaded operators to compare, and
  /// you did it) you will get the original object
  /// in the Hash Table with all the data that you want
  /// to query, the object returned is not
  /// mutable at least that you explicitly declare
  /// other member data mutable, except primary key
  const Type& find(const Type& data) {
    if (!mHf) throw std::runtime_error("Hash function was not defined");

    auto& cont = mHash[mHf(data) % Size];
    auto  elem = cont.find(data);

    if (elem == cont.end())
      throw std::runtime_error("Element was not found");

    return *elem;
  }

  /// Dark magic
  bool update(const Type& oldest, const Type& newest) {
    if (!remove(oldest)) return false;

    if (!add(newest)) {
      add(oldest);
      return false;
    }

    return true;
  }

  void print(std::ostream& os = std::cout) {
    for (std::size_t ii = 0; ii < Size; ++ii) {
      std::cout << "[" << ii << "] => { ";
      for (auto jj = mHash[ii].begin(); jj != mHash[ii].end(); ++jj) {
        std::cout << *jj;

        if (std::next(jj) != mHash[ii].end())
          std::cout << ", ";
      }
      std::cout << " }\n";
    }
  }

  std::size_t get_size() { return Size; }
};

}

#endif
