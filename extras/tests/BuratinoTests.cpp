// Native tests for Buratino. Run: extras\tests\run.ps1
// (or: g++ -std=c++14 -Isrc extras/tests/BuratinoTests.cpp && ./a.out)
#include <cstdio>
#include <cstdlib>
#include <new>
#include "Buratino.h"

using namespace Buratino;

static int failures = 0;
#define CHECK(cond) \
  do { if (!(cond)) { std::printf("  FAIL: %s (line %d)\n", #cond, __LINE__); ++failures; } } while (0)

static bool failAlloc = false;
void* operator new[](size_t size, const std::nothrow_t&) noexcept {
  return failAlloc ? nullptr : std::malloc(size ? size : 1);
}
void operator delete[](void* p) noexcept { std::free(p); }

static void fixedListNeverAllocates() {
  int items[3];
  BList<int> list(items, 0, 3);
  failAlloc = true;
  CHECK(list.Add(1) && list.Add(3) && list.Insert(1, 2));
  CHECK(!list.Add(4));
  CHECK(!list.Insert(0, 0));
  CHECK(!list.Resize(10));
  failAlloc = false;
  CHECK(list.Length() == 3 && items[0] == 1 && items[1] == 2 && items[2] == 3);
}

static void heapListGrows() {
  BList<int> list;
  CHECK(list.Capacity() == 0);
  for (int i = 0; i < 10; ++i) CHECK(list.Add(i));
  CHECK(list.Length() == 10 && list[9] == 9);
  CHECK(list.Insert(0, -1) && list[0] == -1 && list[10] == 9);
  CHECK(list.Insert(list.Length(), 99) && list[11] == 99);
  CHECK(!list.Insert(list.Length() + 1, 5));
}

static void heapListReportsOutOfMemory() {
  BList<int> list(1);
  CHECK(list.Add(1));
  failAlloc = true;
  CHECK(!list.Add(2));
  CHECK(!list.Insert(0, 2));
  failAlloc = false;
  CHECK(list.Length() == 1 && list[0] == 1);
}

static void removeIgnoresBadIndex() {
  BList<int> list;
  list.Remove(0);
  CHECK(list.Length() == 0);
  list.Add(1); list.Add(2); list.Add(3);
  list.Remove(1);
  list.Remove(5);
  CHECK(list.Length() == 2 && list[0] == 1 && list[1] == 3);
}

static int alive = 0;
struct Tracked { Tracked() { ++alive; } ~Tracked() { --alive; } };

static void ptrDeletesWithLastReference() {
  {
    BPtr<Tracked> a(new Tracked);
    {
      BPtr<Tracked> b(a);
      BPtr<Tracked> c;
      c = b;
      CHECK(alive == 1 && c == a);
    }
    CHECK(alive == 1);
    a = BPtr<Tracked>();
    CHECK(alive == 0 && !a);
  }
  CHECK(alive == 0);
}

int main() {
  fixedListNeverAllocates();
  heapListGrows();
  heapListReportsOutOfMemory();
  removeIgnoresBadIndex();
  ptrDeletesWithLastReference();
  std::printf(failures ? "%d check(s) failed.\n" : "All tests passed.\n", failures);
  return failures ? 1 : 0;
}
