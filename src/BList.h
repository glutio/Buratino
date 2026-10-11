#ifndef BURATINO_BLIST_H
#define BURATINO_BLIST_H

#include <stddef.h>
#include <limits.h>
#include <new>

namespace Buratino {

// A small array list with two kinds of storage:
//   fixed: wraps an array you own; never allocates, Add/Insert fail when full
//     BView* items[4];
//     BList<BView*> list(items, 0, 4);
//   heap: grows by doubling; Add/Insert fail if memory runs out
//     BList<BView*> list;      // allocates on first Add
//     BList<BView*> list(8);   // allocates 8 slots now
template<typename T>
class BList {
protected:
  T* _array;
  unsigned _count;
  unsigned _capacity;
  bool _owner;

  bool grow() {
    if (!_owner || _capacity > UINT_MAX / 2) {
      return false;
    }
    return Resize(_capacity ? _capacity * 2 : 2);
  }

public:
  // Fixed storage over `array`, of which the first `count` items are in use.
  BList(T* array, unsigned count, unsigned capacity)
    : _array(array), _count(count), _capacity(capacity), _owner(false) {}

  explicit BList(unsigned capacity)
    : _array(0), _count(0), _capacity(0), _owner(true) { Resize(capacity); }

  BList()
    : _array(0), _count(0), _capacity(0), _owner(true) {}

  // Copying would share (and double-delete) the array.
  BList(const BList&) = delete;
  BList& operator = (const BList&) = delete;

  ~BList() {
    if (_owner) {
      delete[] _array;
    }
  }

  T& operator[](unsigned index) {
    return _array[index];
  }

  bool Add(T item) {
    if (_count == _capacity && !grow()) {
      return false;
    }
    _array[_count++] = item;
    return true;
  }

  bool Insert(unsigned index, T item) {
    if (index > _count || (_count == _capacity && !grow())) {
      return false;
    }
    for (unsigned i = _count; i > index; --i) {
      _array[i] = _array[i - 1];
    }
    _array[index] = item;
    ++_count;
    return true;
  }

  void Remove(unsigned index) {
    if (index >= _count) {
      return;
    }
    for (unsigned i = index; i + 1 < _count; ++i) {
      _array[i] = _array[i + 1];
    }
    --_count;
  }

  unsigned Length() {
    return _count;
  }

  unsigned Capacity() {
    return _capacity;
  }

  // Heap storage only. Shrinking drops the items past the new capacity.
  bool Resize(unsigned capacity) {
    if (!_owner || capacity > static_cast<size_t>(-1) / sizeof(T)) {
      return false;
    }
    T* array = capacity ? new (std::nothrow) T[capacity] : 0;
    if (capacity && !array) {
      return false;
    }
    unsigned count = capacity < _count ? capacity : _count;
    for (unsigned i = 0; i < count; ++i) {
      array[i] = _array[i];
    }
    delete[] _array;
    _array = array;
    _capacity = capacity;
    _count = count;
    return true;
  }
};

}

#endif
