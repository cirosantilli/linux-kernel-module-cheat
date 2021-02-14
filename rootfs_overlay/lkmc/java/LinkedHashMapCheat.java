/*
# LinkedHashMap

http://docs.oracle.com/javase/7/docs/api/java/util/LinkedHashMap.html

Hash map that is iterable in insertion order.

Application LRU cache:

- https://github.com/haoel/leetcode/pull/90/files
- http://stackoverflow.com/questions/23772102/lru-cache-in-java-with-generics-and-o1-operations

This is a sub-case of a binary heap: it is efficient
when every item update makes it either the most recent, or oldest.
For more general binary heap, the new item can go anywhere.

# removeEldestEntry

Example:
https://github.com/cirosantilli/haoel-leetcode/commit/ff04930b2dc31f270854e40b560723577c7b49fd

Only acts on `put`, `get` does not update values for us.
*/

import java.util.LinkedList;
import java.util.LinkedHashMap;
import java.util.Iterator;

public class LinkedHashMapCheat {
    public static void main(String[] args) {
        LinkedList<Integer> output;
        LinkedList<Integer> expected;
        Iterator<Integer> it;

        LinkedHashMap<Integer,Integer> m = new LinkedHashMap<>();
        assert m.put(2, -2) == null;
        assert m.put(1, -1) == null;
        assert m.put(3, -3) == null;
        output = new LinkedList<>();
        expected = new LinkedList<>();
        expected.add(2);
        expected.add(1);
        expected.add(3);
        for (int i : m.keySet())
            output.add(i);
        assert output.equals(expected);

        it = m.keySet().iterator();
        it.next();
        it.remove();
        output = new LinkedList<>();
        expected = new LinkedList<>();
        expected.add(1);
        expected.add(3);
        for (int i : m.keySet())
            output.add(i);
        assert output.equals(expected);
    }
}
