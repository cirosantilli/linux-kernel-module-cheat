// https://cirosantilli.com/linux-kernel-module-cheat#cpp
//
// http://www.cplusplus.com/reference/map/map/
//
// Also comes in an unordered version `unordered_map`.
//
// Ordered.
//
// Also comes in an multiple value input version `multimap`.
//
// Does not require a hash function. Usually implemented as a self balancing tree such as a rb tree.
//
// # hashmap
//
// There seems to be no explicit hashmap container, only a generic map interface,
//
// However unordered_map is likely to be hashmap based.
//
// A nonstandard `hash_map` already provided with gcc and msvc++.
// It is placed in the `std::` namespace, but it is *not* ISO.

#include <cassert> // map, multimap
#include <map> // map, multimap
#include <sstream> // stringstream

template <class K, class V>
std::string map_to_str(std::map<K,V> map) {
    std::stringstream result;
    for (auto& pair : map) {
        result << pair.first << ":" << pair.second << ", ";
    }
    return result.str();
}

int main() {
    // Initializer list constructor.
    {
        std::map<int,std::string> m{
            {0, "zero"},
            {1, "one"},
        };
        assert(m.at(0) == "zero");
        assert(m.at(1) == "one");
    }

    // # emplace
    //
    // Put a value pair into the map without creating the pair explicitly.
    //
    // Only inserts if not already present.
    //
    // Needs gcc 4.8: http://stackoverflow.com/questions/15812276/stdset-has-no-member-emplace
    {
        std::map<int,std::string> m;
        assert((m.emplace(0, "zero").second));
        assert((m.emplace(1, "one").second));
        assert(!(m.emplace(1, "one2").second));
        assert(m.at(0) == "zero");
        assert(m.at(1) == "one");
    }

    // # operator[]
    //
    // Get value from a given key.
    //
    // Creates if not present, so be very careful if that's not what you want!
    //
    // Use:
    //
    // - this to "add new or update existing" or "create default value and return it"
    // - at() to find when you are sure it is there
    // - find() to find when you are not sure it is there
    // - emplace() for putting new values when you are sure they are not there
    {
        std::map<int,std::string> m{
            {0, "zero"},
            {1, "one"},
        };
        assert(m[0] == "zero");
        assert(m[1] == "one");

        // Returns a reference that can override the value.
        m[1] = "one2";
        assert(m[1] == "one2");

        // WARNING: if the key does not exist, it is inserted with a value with default constructor.
        // This can be avoided by using `find` or `at` instead of `[]`.
        // Inserts `(2,"")` because `""` is the value for the default String constructor.
        // http://stackoverflow.com/questions/10124679/what-happens-if-i-read-a-maps-value-where-the-key-does-not-exist
        {
            assert(m[2] == "");
            assert(m.size() == 3);

            // This behaviour is however very convenient for nested containers.
            {
                std::map<int,std::map<int,int>> m;
                // Create the empty map at m[0], and immediately add a (0,0) pair to it.
                m[0][0] = 0;
                // map at m[0] already exists, now just add a new (1, 1) pair to it.
                m[0][1] = 1;
                m[1][0] = 2;
                assert(m[0][0] == 0);
                assert(m[0][1] == 1);
                assert(m[1][0] == 2);
            }
        }
    }

    // # find
    //
    // Similar to `std::set` find with respect to the keys:
    // returns an iterator pointing to the pair which has given key, not the value.
    //
    // If not found, returns `map::end()`
    //
    // This is preferable to `[]` since it does not insert non-existent elements.
    {
        std::map<int,std::string> m{
            {0, "zero"},
            {1, "one"},
        };

        auto found = m.find(0);
        assert(found != m.end());
        assert(found->first == 0);
        assert(found->second == "zero");

        assert(m.find(2) == m.end());
        assert(m.size() == 2);

        // https://stackoverflow.com/questions/2333728/stdmap-default-valueGet a default provided value if key not present
        //
        // TODO: any less verbose way than finding and check != end? Like:
        //
        // m.get(key, default)
        {
            std::map<int,int> m{};
            int default_ = 42;
            int result;
            auto f = m.find(1);
            if (f == m.end()) {
                result = default_;
            } else {
                result = f->second;
            }
            assert(result == 42);
        }
    }

    // # at
    //
    // A convenient version of find() that returns the item directly.
    //
    // Throws if not present, so better when the key is supposed to be there.
    //
    // C++11.
    {
        std::map<int,std::string> m{
            {0, "zero"},
            {1, "one"},
        };
        // Returns a reference, so we can modify it.
        m.at(1) = "one2";
        assert(m.at(1) == "one2");
    }

    // # insert
    //
    // Insert pair into map.
    //
    // The return value is similar to that of a set insertion with respect to the key.
    //
    // Just use emplace instead, less verbose as it was added after perfect forwarding.
    //
    // http://stackoverflow.com/questions/17172080/insert-vs-emplace-vs-operator-in-c-map
    {
        std::map<int,std::string> m;
        std::pair<std::map<int,std::string>::iterator,bool> ret;

        ret = m.insert(std::make_pair(0, "zero"));
        assert(ret.first == m.find(0));
        assert(ret.second == true);

        ret = m.insert(std::make_pair(1, "one"));
        assert(ret.first == m.find(1));
        assert(ret.second == true);

        //key already present
        ret = m.insert(std::make_pair(1, "one2"));
        assert(m[1] == "one");
        assert(ret.first == m.find(1));
        assert(ret.second == false);
    }

    // # iterate
    //
    // Map is ordered:
    // http://stackoverflow.com/questions/7648756/is-the-order-of-iterating-through-stdmap-known-and-guaranteed-by-the-standard
    //
    // It is iterated in key `<` order.
    //
    // So this basically requires implementations to use balanced
    // trees intead of hashmap.
    //
    // Iteration returns key value pairs.
    {
        std::map<int,std::string> m{
            {1, "one"},
            {0, "zero"},
        };

        int i = 0;
        int is[] = {0, 1};
        for (auto& im : m) {
            assert(im.first == is[i]);
            //cout << im->second << endl;
            ++i;
        }
        assert(i == 2);
        assert(map_to_str(m) == "0:zero, 1:one, ");
    }

    // # erase
    //
    // Remove element from map.
    {
        // key version. Returns number of elements removed (0 or 1).
        {
            std::map<int,std::string> m{
                {0, "zero"},
                {1, "one"},
            };
            int ret;
            ret = m.erase(1);
            assert(ret = 1);
            assert((m == std::map<int,std::string>{{0, "zero"}}));

            ret = m.erase(1);
            assert(ret == 0);
        }

        // iterator version. Returns iterator to next element.
        // Does not invalidate other iterators.
        // http://stackoverflow.com/questions/6438086/iterator-invalidation-rules
        {
            std::map<int,std::string> m{
                {0, "zero"},
                {1, "one"},
            };
            auto itNext = m.find(1);
            auto it = m.find(0);
            assert(m.erase(it) == itNext);
            assert((m == std::map<int,std::string>{{1, "one"}}));
        }
    }
}
