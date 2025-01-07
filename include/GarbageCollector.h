#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <Scope.h>
#include <stack>
#include <thread>

struct GCRequest {
  Scope *scope;
};

class GarbageCollector {
private:
  std::vector<std::unique_ptr<HeapValue> > globalHeap;

  std::vector<Value> *stackRef = nullptr;
  Scope *topScopeChain = nullptr;

  void mark(const std::vector<HeapValue*>& heapRefs, Scope* topScope);
  void markValue(HeapValue* ref);
  void sweep();

public:
  HeapValue *allocObject(std::unique_ptr<HeapValue> hv, Scope *scope);

  void collectGarbage(const std::vector<HeapValue *> &heapRefs, Scope *topScope);
};
