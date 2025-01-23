#pragma once

#include <cstddef>

namespace Luna {
  /**
   * A fixed-size pool of objects.
   */
  template <typename T, size_t N> class Pool {
    public:
    class Iterator {
      public:
      using iterator_category = std::input_iterator_tag;
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using pointer = T *;
      using reference = T &;

      Iterator(Pool *pool, size_t index) : mPool(pool), mIndex(index) {}

      T *operator->() { return &mPool->at(mIndex); }

      T &operator*() { return mPool->at(mIndex); }

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

      bool operator==(const Iterator &other) { return mIndex == other.mIndex; }

      bool operator!=(const Iterator &other) { return mIndex != other.mIndex; }

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

      ConstIterator(const Pool *pool, size_t index)
          : mPool(pool), mIndex(index) {}

      const T *operator->() { return &mPool->at(mIndex); }

      const T &operator*() { return mPool->at(mIndex); }

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
          this->at(i).~T();
        }
      }
    }

    /**
     * Acquire an object from the pool.
     * 
     * @return A pointer to the object, or nullptr if the pool is full.
     */
    template <typename... ArgTypes> T *acquire(ArgTypes... args) {
      for (size_t i = 0; i < N; i++) {
        if (!mInUse[i]) {
          mInUse[i] = true;
          new (&this->at(i)) T(args...);

          return &this->at(i);
        }
      }

      return nullptr;
    }

    void release(T *object) {
      for (size_t i = 0; i < N; i++) {
        if (&this->at(i) == object) {
          mInUse[i] = false;
          this->at(i).~T();

          break;
        }
      }
    }

    T &at(size_t index) {
      return *reinterpret_cast<T *>(&mData[sizeof(T) * index]);
    }

    const T &at(size_t index) const {
      return *reinterpret_cast<const T *>(&mData[sizeof(T) * index]);
    }

    T &operator[](size_t index) { return this->at(index); }

    const T &operator[](size_t index) const { return this->at(index); }

    Iterator begin() {
      size_t index = 0;

      while (index < N && !mInUse[index]) {
        index++;
      }

      return Iterator(this, index);
    }

    Iterator end() { return Iterator(this, N); }

    ConstIterator begin() const {
      size_t index = 0;

      while (index < N && !mInUse[index]) {
        index++;
      }

      return ConstIterator(this, index);
    }

    ConstIterator end() const { return ConstIterator(this, N); }

    private:
    std::array<std::byte, sizeof(T) * N> mData;
    std::array<bool, N> mInUse{false};
  };
} // namespace Luna
