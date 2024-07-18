#pragma once

#include <functional>
#include <future>
#include <queue>

/**
 * @page resources Resources
 *
 * A Resource holds information on how to load something.
 * This can be an image, a font, a text or virtually anything.
 *
 * Many classes hold references to resources, so they are loaded
 * and freed on demand.
 * One particular use case is the Sprite and an Image. The Sprite
 * only holds an Image Resource (`ImageResPtr`, aka
 * `shared_ptr<Resource<Image>>`). When the active renderer of a canvas
 * (let's say, OpenGL) wants to draw the sprite, it knows that it didn't load
 * the referenced image as a texture into GPU memory and requests loading this.
 * When the image is loaded into CPU memory, the renderer uploads the image
 * into its internal GPU memory, frees the reference to the loaded Image and
 * the Image is unloaded from the CPU memory as there are no more references
 * to the Image shared pointer.
 * When you now switch to another renderer (let's say, Vulkan), it won't have
 * the texture in its context anymore. But instead of restarting the whole
 * application in order to load all the textures, it can follow the same steps
 * as before and ask the Resource to load the data temporarily.
 *
 * In code, you can do something like this:
 *
 * ```cpp
 * class MyImageLoader {
 *   public:
 *   MyImageLoader(Vector2i size) : mSize(size) {}
 *
 *   // this is the important one to make the magic work
 *   ImagePtr operator()() {
 *     // in a real scenario, you would load the image data from somewhere and
 *     // fill the Image accordingly before returning it
 *     return Image::makeRgb32(mSize);
 *   }
 *
 *   private:
 *   Vector2i mSize;
 * };
 *
 * void example() {
 *   ImageResPtr resource = make_shared<Resource<Image>>(MyImageLoader{32, 32});
 *   // resource can be assigned to sprites, materials etc.
 *
 *   // if a code, for example a renderer, needs access to the data
 *   Resource<Image>::FutureType future = resource->get();
 *
 *   // since we are the first requesting the future, loading is initiated in
 *   // the background (separate thread) if supported or loaded immediately
 *   // it's important to keep reference to the future, else the data is
 *   // immediately disposed after loading
 *
 *   // every frame, we can check if the resource is loaded
 *   if (resource->isReady()) {
 *     ImagePtr image = future.get();
 *
 *     // you can dispose future now
 *     // if there are no more references to future or image, the image is
 *     // automatically disposed from memory
 *   }
 * }
 * ```
 *
 * The following table visualizes the relationship between the items:
 *
 * @dot
 * digraph mygraph {
 *   "Resource<Image>" -> "MyImageLoader"
 *   "Resource<Image>" -> "Future<ImagePtr>"
 *   "MyImageLoader" -> "ImagePtr"
 *   "Future<ImagePtr>" -> "ImagePtr"
 * }
 * @enddot
 */

namespace Luna {
#ifdef N64
  template <typename T> class Promise;

  template <typename T> class Future {
    public:
    T get() { return std::move(*mValue); }

    private:
    std::shared_ptr<T> mValue;
    Future() : mValue{std::make_shared<T>()} {}
    friend class Promise<T>;
  };

  template <typename T> class Promise {
    public:
    Future<T> get_future() {
      Future<T> future;
      mValue = future.mValue;
      return future;
    }

    void set_value(const T &value) { *mValue = value; }

    private:
    std::shared_ptr<T> mValue;
  };
#endif

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

#ifdef N64
    using FutureType = Future<ResultType>;

    using PromiseType = Promise<ResultType>;
#else
    using FutureType = std::future<ResultType>;

    using PromiseType = std::promise<ResultType>;
#endif

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
