// https://cirosantilli.com/linux-kernel-module-cheat#cpp-iterators

#include <cassert>
#include <map>
#include <iostream>

class MyMap {
  public:
    std::map<int,int> map;
    class iterator {
        using It = decltype(map)::iterator;
        It it;
      public:
        using difference_type = It::difference_type;
        using value_type = It::value_type;
        using pointer = It::pointer;
        using reference = It::reference;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(It begin) : it(begin) {}
        iterator& operator++() {it++; return *this;}
        iterator operator++(int) {auto retval = *this; ++(*this); return retval;}
        iterator& operator--() {it--; return *this;}
        iterator operator--(int) {auto retval = *this; --(*this); return retval;}
        bool operator==(iterator other) const { return it == other.it; }
        bool operator!=(iterator other) const { return !(*this == other); }
        value_type operator*() {
            auto pair = *it;
            return std::make_pair(2*pair.first, 3*pair.second);
        }
    };
    iterator begin() { return iterator(map.begin()); }
    iterator end() { return iterator(map.end()); }
};

int main() {
    MyMap map;
    map.map.emplace(0, 10);
    map.map.emplace(1, 11);
    map.map.emplace(2, 12);

    auto it = map.begin();
    assert((*it++ == std::pair<const int, int>(0, 30)));
    assert((*it++ == std::pair<const int, int>(2, 33)));
    assert((*it++ == std::pair<const int, int>(4, 36)));

    for (const auto& v : map) {
        std::cout << v.first << " " << v.second << std::endl;
    }
}
