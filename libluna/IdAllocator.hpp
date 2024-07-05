#pragma once

#include <set>
#include <stdexcept>

namespace Luna {
  template <typename T>
  class IdAllocator {
    public:
    T next() {
      if (!mFreeIds.empty()) {
        auto it = mFreeIds.begin();
        T id = *it;
        mFreeIds.erase(it);
        return id;
      } else {
        if (mNextId == 0) {
          throw std::runtime_error("ID overflow");
        }

        return mNextId++;
      }
    }

    void free(T id) {
      if (id == 0 || id >= mNextId || mFreeIds.count(id) > 0) {
        throw std::invalid_argument("Invalid ID");
      }

      mFreeIds.insert(id);
    }

    private:
    T mNextId{1};
    std::set<T> mFreeIds;
  };
}
