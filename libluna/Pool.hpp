#pragma once

#include <cstddef>

namespace Luna {
  /**
   * A fixed-size pool of objects.
   */
  template<typename T, size_t N>
  class Pool {
    public:
    class Iterator {
      public:
      using iterator_category = std::input_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using pointer = T *;
      using reference = T &;

      Iterator(Pool *pool, size_t index) : mPool(pool), mIndex(index) {}

      T *operator->() {
        return &mPool->mObjects[mIndex];
      }

      T &operator*() {
        return mPool->mObjects[mIndex];
      }

      Iterator &operator++() {
        do {
          mIndex++;
        } while (mIndex < N && !mPool->mInUse[mIndex]);

        return *this;
      }

      Iterator operator++(int) {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
      }

      bool operator==(const Iterator &other) {
        return mIndex == other.mIndex;
      }

      bool operator!=(const Iterator &other) {
        return mIndex != other.mIndex;
      }

      private:
      Pool *mPool;
      size_t mIndex;
    };

    class ConstIterator {
      public:
      using iterator_category = std::input_iterator_tag;
      using value_type = const T;
      using difference_type = std::ptrdiff_t;
      using pointer = const T *;
      using reference = const T &;

      ConstIterator(const Pool *pool, size_t index) : mPool(pool), mIndex(index) {}

      const T *operator->() {
        return &mPool->mObjects[mIndex];
      }

      const T &operator*() {
        return mPool->mObjects[mIndex];
      }

      ConstIterator &operator++() {
        do {
          mIndex++;
        } while (mIndex < N && !mPool->mInUse[mIndex]);

        return *this;
      }

      ConstIterator operator++(int) {
        ConstIterator tmp = *this;
        ++(*this);
        return tmp;
      }

      bool operator==(const ConstIterator &other) {
        return mIndex == other.mIndex;
      }

      bool operator!=(const ConstIterator &other) {
        return mIndex != other.mIndex;
      }

      private:
      const Pool *mPool;
      size_t mIndex;
    };

    ~Pool() {
      for (size_t i = 0; i < N; i++) {
        if (mInUse[i]) {
          mObjects[i].~T();
        }
      }
    }

    /**
     * Acquire an object from the pool.
     * 
     * @return A pointer to the object, or nullptr if the pool is full.
     */
    T *acquire() {
      for (size_t i = 0; i < N; i++) {
        if (!mInUse[i]) {
          mInUse[i] = true;
          new (&mObjects[i]) T();

          return &mObjects[i];
        }
      }

      return nullptr;
    }

    void release(T *object) {
      for (size_t i = 0; i < N; i++) {
        if (&mObjects[i] == object) {
          mInUse[i] = false;
          mObjects[i].~T();

          break;
        }
      }
    }

    Iterator begin() {
      size_t index = 0;

      while (index < N && !mInUse[index]) {
        index++;
      }

      return Iterator(this, index);
    }

    Iterator end() {
      return Iterator(this, N);
    }

    ConstIterator begin() const {
      size_t index = 0;

      while (index < N && !mInUse[index]) {
        index++;
      }

      return ConstIterator(this, index);
    }

    ConstIterator end() const {
      return ConstIterator(this, N);
    }

    private:
    std::array<T, N> mObjects;
    std::array<bool, N> mInUse{false};
  };
}
