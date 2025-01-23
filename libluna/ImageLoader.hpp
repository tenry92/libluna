#pragma once

#include <libluna/Image.hpp>

namespace Luna {
  class ImageLoader {
    public:
    virtual ~ImageLoader() = default;

    virtual Image load() = 0;
  };
} // namespace Luna
