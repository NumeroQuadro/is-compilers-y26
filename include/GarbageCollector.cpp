#include "GarbageCollector.h"
#include <algorithm>

HeapValue *GarbageCollector::allocObject(std::unique_ptr<HeapValue> hv) {
  HeapValue *raw = hv.get();
  globalHeap.push_back(std::move(hv));
  return raw;
}

void GarbageCollector::collectGarbage(const std::vector<HeapValue *> &heapRefs, const Scope *topScope) {
  mark(heapRefs, topScope);
  sweep();
}


void GarbageCollector::mark(const std::vector<HeapValue *> &heapRefs, const Scope *topScope) {
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
