#pragma once

#include <memory>
#include <vector>

#include <libluna/Image.hpp>
#include <libluna/InputStream.hpp>

namespace Luna::Util {
  class Png {
    public:
    Png(std::unique_ptr<Luna::InputStream> reader);
    ~Png();

    std::shared_ptr<Image>
    decode(int framesPerRow = 1, int framesPerColumn = 1);

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna::Util
