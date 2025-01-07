#include "GarbageCollector.h"
#include <algorithm>

HeapValue *GarbageCollector::allocObject(std::unique_ptr<HeapValue> hv, Scope *scope) {
  HeapValue *raw = hv.get();
  globalHeap.push_back(std::move(hv));
  if (scope && !scope->isClosed) {
    scope->objectsInThisScope.push_back(raw);
  }
  return raw;
}

void GarbageCollector::collectGarbage(const std::vector<HeapValue *> &heapRefs,Scope *topScope) {
  mark(heapRefs, topScope);
  sweep();
}


void GarbageCollector::mark(const std::vector<HeapValue *> &heapRefs, Scope *topScope) {
  const Scope *s = topScope;
  while (s) {
    for (const auto &[fst, snd]: s->values) {
      if (snd.isHeapRef()) {
        markValue(snd.asHeapRef());
      }
    }
    s = s->previous_;
  }

  for (auto *ref: heapRefs) {
    markValue(ref);
  }
}

void GarbageCollector::markValue(HeapValue *ref) {
  if (!ref || ref->marked) return;
  ref->marked = true;
  ref->markChildren();
}

void GarbageCollector::sweep() {
  for (auto it = globalHeap.begin(); it != globalHeap.end();) {
    if ((*it)->marked) {
      (*it)->marked = false;
      ++it;
    } else {
      it = globalHeap.erase(it);
    }
  }
}
