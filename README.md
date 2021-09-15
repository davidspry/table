# table

A simple array type that provides a virtual grid topology.

- An element can be erased from the grid in constant time;
- An element can be retrieved using a grid position in constant time;
- The table's elements can be iterated over directly (i.e., linear in the number of elements rather than in the size of the table), and;
- The table can be resized dynamically.

## Usage example

```cpp
#include "table.hpp"

// 1. Create a 2x2 table
ds::table<int> table(2, 2);

// 2. Emplace two items
table.emplace(0, 0, 4);
table.emplace(1, 1, 8);

//  | 4 | _ |
//  | _ | 8 |

// 3. Enlarge the table
table.set_size(5, 4);

//  | 4 | _ | _ | _ |
//  | _ | 8 | _ | _ |
//  | _ | _ | _ | _ |
//  | _ | _ | _ | _ |
//  | _ | _ | _ | _ |

table.emplace(2, 2, 0);

//  | 4 | _ | _ | _ |
//  | _ | 8 | _ | _ |
//  | _ | _ | 0 | _ |
//  | _ | _ | _ | _ |
//  | _ | _ | _ | _ |

// 4. Shrink the table
table.set_size(3, 3);

//  | 4 | _ | _ |
//  | _ | 8 | _ |
//  | _ | _ | 0 |

// 5. Safely access table cells
assert(table.get(0, 2) == nullptr);
assert(table.at_else(0, 0, -1) ==  4);
assert(table.at_else(0, 2, -1) == -1);

// 6. Erase elements by grid position
table.erase(1, 1);

//  | 4 | _ | _ |
//  | _ | _ | _ |
//  | _ | _ | 0 |

// 7. Iterate over data items
for (auto & item : table) {
  assert(item == 0 || item == 4);
}
```
