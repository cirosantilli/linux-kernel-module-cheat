// https://cirosantilli.com/linux-kernel-module-cheat#cpp

#include <cassert>
#include <set>

int main() {
    std::set<int> s{0, 2};

    // Insert another element.
    auto insert_ret = s.insert(3);
    assert(insert_ret.first == s.find(3));
    assert(insert_ret.second == true);

    // Insert something already there, second of return is false.
    insert_ret = s.insert(3);
    assert(insert_ret.second == false);

    // Check if a value is present.
    auto zero = s.find(0);
    assert(zero != s.end());
    assert(*zero == 0);

    // Now for a value that is not present.
    auto one = s.find(1);
    assert(one == s.end());

#if __cplusplus > 201703L
    // In C++20 we finally have a .contains() to avoid the end mess!
    assert(s.contains(0));
    assert(!s.contains(1));
#endif

    // Remove an element.
    // Was present, so return 1;
    assert(s.erase(0) == 1);
    assert(s.find(0) == s.end());
    // Not present, return 0.
    assert(s.erase(0) == 0);
}
