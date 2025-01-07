#pragma once
#include <Scope.h>
#include <thread>


class GarbageCollector {
private:
  std::vector<std::unique_ptr<HeapValue> > globalHeap;

  void mark(const std::vector<HeapValue *> &heapRefs, const Scope *topScope);

  void markValue(HeapValue *ref);

  void sweep();

public:
  HeapValue *allocObject(std::unique_ptr<HeapValue> hv);

  void collectGarbage(const std::vector<HeapValue *> &heapRefs, const Scope *topScope);
};
