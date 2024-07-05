#pragma once

#include <functional>
#include <future>
#include <queue>

namespace Luna {
  template <typename T> class Resource;
  template <typename T> using ResourcePtr = std::shared_ptr<Resource<T>>;

  /**
   * @brief Resource for images, levels, audio and more.
   *
   * Some implementations load the whole resource into the memory
   * (e.g. many image loaders), some open a streaming reference
   * (e.g. many audio loaders).
   *
   * After acquiring the reference to a resource the first time, the loading is
   * usually scheduled in a separate queue. Make sure to check `isReady()`
   * before accessing any of the resource's data.
   */
  template <typename T> class Resource {
    public:
    /**
     * @brief The data type that is returned after the resource is loaded.
     *
     * This is a shared pointer to the actual resource object.
     */
    using ResultType = std::shared_ptr<T>;

    /**
     * @brief Type for loading the resource of ResultType.
     */
    using LoaderType = std::function<ResultType()>;

    /**
     * @brief Future for a ResultType.
     */
    using FutureType = std::future<ResultType>;

    using PromiseType = std::promise<ResultType>;

    /**
     * @brief Instantiate a new resource using the given loader.
     *
     * The loader will not be called until a user is interested in retrieving
     * the actual resource.
     */
    inline Resource(LoaderType loader) { mLoader = loader; }

    /**
     * @brief Get the future for this resource.
     *
     * If not initiated yet, this will trigger loading the actual resource.
     * If the resource is already loaded, the future resolved immediately.
     */
    inline FutureType get() {
      PromiseType promise;
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
    std::queue<PromiseType> mPromises;
    std::weak_ptr<T> mResource;
    LoaderType mLoader;
  };
} // namespace Luna
