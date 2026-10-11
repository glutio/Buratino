# Buratino

Shared building blocks for the [Eventfun](https://github.com/glutio/Eventfun),
[Taskfun](https://github.com/glutio/Taskfun) and
[Focuspocus](https://github.com/glutio/Focuspocus) Arduino libraries. They
declare `depends=Buratino`, so the Library Manager installs it for you.

```cpp
#include <Buratino.h>
using namespace Buratino;
```

## BList

A small array list with two kinds of storage.

```cpp
// Fixed: wraps an array you own. Never allocates; Add/Insert return false when full.
int items[4];
BList<int> fixed(items, 0, 4);   // array, items in use, capacity

// Heap: grows by doubling. Add/Insert return false if memory runs out.
BList<int> heap;                 // allocates on the first Add
BList<int> heap8(8);             // allocates 8 slots now
```

| Method | |
|---|---|
| `bool Add(item)` | append |
| `bool Insert(index, item)` | insert before `index` (`index == Length()` appends) |
| `void Remove(index)` | remove and shift; a bad index is ignored |
| `T& operator[](index)` | no bounds check |
| `unsigned Length()`, `Capacity()` | |
| `bool Resize(capacity)` | heap storage only |

A `BList` can't be copied.

## BPtr

A reference-counted pointer: the object is deleted with the last `BPtr`
that points to it.

```cpp
BPtr<Thing> a(new Thing);
BPtr<Thing> b = a;   // shared
a->doSomething();
```

## Tests

`extras\tests\run.ps1` builds and runs the native tests with MSVC.

## License

MIT
