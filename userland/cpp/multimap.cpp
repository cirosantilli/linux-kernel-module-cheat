// https://cirosantilli.com/linux-kernel-module-cheat#cpp
//
// Map where a key can have multiple values.
//
// Simple to implement with map of set or vector:
//
// - http://stackoverflow.com/questions/8602068/whats-the-difference-between-stdmultimapkey-value-and-stdmapkey-stds
// - http://stackoverflow.com/questions/4437862/whats-the-advantage-of-multimap-over-map-of-vectors

#include <cassert> // map, multimap
#include <map> // map, multimap
#include <sstream> // stringstream
#include <vector> // stringstream

int main() {
    // equal_range iterates over all key value pairs with a given key.
    {
        std::multimap<int, int> m{
            {1, 2},
            {1, 3},
            {2, 4}
        };
        auto range = m.equal_range(1);
        auto it = range.first;
        assert(it->second == 2);
        it++;
        assert(it->second == 3);
        it++;
        assert(it == range.second);
    }

    // Iteration over map iterates all pairs.
    {
        std::multimap<int, int> m{
            {1, 2},
            {1, 3},
            {2, 4}
        };

        auto it = m.begin();
        assert(it->first == 1);
        assert(it->second == 2);

        it++;
        assert(it->first == 1);
        assert(it->second == 3);

        it++;
        assert(it->first == 2);
        assert(it->second == 4);

        it++;
        assert(it == m.end());
    }

    // # Iterate each key only once
    //
    // # Group by key
    //
    // - https://stackoverflow.com/questions/9371236/is-there-an-iterator-across-unique-keys-in-a-stdmultimap/41523639#41523639
    // - http://stackoverflow.com/questions/247818/stlmultimap-how-do-i-get-groups-of-data
    {
        std::multimap<int, int> m{
            {1, 2},
            {1, 3},
            {2, 4}
        };
        std::vector<int> out;

        //for (auto it = m.begin(), end = m.end(); it != end;) {
            //auto key = it->first;
            //out.push_back(key);
            //do {
                //it++;
            //} while (it != end && it->first == key);
        //}

        auto it = m.begin();
        auto end = m.end();
        auto pair = *it;
        auto key = pair.first;
        while (true) {
            // Operate on key.
            out.push_back(key);
            decltype(key) next_key;
            do {
                // Operate on value.
                it++;
                if (it == end) {
                    goto end;
                }
                next_key = it->first;
            } while (next_key == key);
            key = next_key;
        }
        end:

        assert(out == std::vector<int>({1, 2}));
    }
}
