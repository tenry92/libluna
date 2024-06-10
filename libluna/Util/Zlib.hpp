#pragma once

#include <memory>
#include <vector>

#include <libluna/InputStream.hpp>

namespace Luna::Util {
  class Zlib {
    public:
    Zlib(std::unique_ptr<Luna::InputStream> reader);
    ~Zlib();

    std::vector<uint8_t> inflate();

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna::Util
