#pragma once

#include <functional>
#include <future>
#include <queue>

namespace Luna {
  /**
   * @brief Resource reference to images, levels, audio and more.
   *
   * Some implementations load the whole resource into the memory
   * (e.g. many image loaders), some open a streaming reference
   * (e.g. many audio loaders).
   *
   * After acquiring the reference to a resource the first time, the loading is
   * usually scheduled in a separate queue. Make sure to check `isReady()`
   * before accessing any of the resource's data.
   */
  template <typename T> class ResourceRef {
    public:
    using ResultType = std::shared_ptr<T>;
    using LoaderType = std::function<ResultType()>;
    using FutureType = std::future<ResultType>;

    inline ResourceRef(LoaderType loader) { mLoader = loader; }

    inline FutureType get() {
      std::promise<ResultType> promise;
      auto future = promise.get_future();

      if (this->isReady()) {
        promise.set_value(mResource.lock());
      } else {
        mPromises.emplace(std::move(promise));

        if (mPromises.size() == 1) {
          /// @todo Load in a separate thread
          // std::async(std::launch::async, [this]() { this->load(); });
          this->load();
        }
      }

      return future;
    }

    /**
     * @brief Check whether the resource is ready to be used.
     */
    inline bool isReady() const { return !mResource.expired(); }

    inline operator bool() const { return isReady(); }

    inline void load() {
      /// @todo Lock mutex
      auto resource = mLoader();
      mResource = resource;

      while (!mPromises.empty()) {
        auto promise = std::move(mPromises.front());
        mPromises.pop();
        promise.set_value(resource);
      }
    }

    private:
    std::queue<std::promise<ResultType>> mPromises;
    std::weak_ptr<T> mResource;
    LoaderType mLoader;
  };
} // namespace Luna
