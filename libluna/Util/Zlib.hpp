#pragma once

#include <memory>
#include <vector>

#include <zlib.h>

#include <libluna/InputStream.hpp>

namespace Luna::Util {
  class Zlib {
    public:
    Zlib(std::unique_ptr<Luna::InputStream> reader);
    ~Zlib();

    std::vector<uint8_t> inflate();

    private:
    void init();
    void end();

    std::unique_ptr<Luna::InputStream> mReader;
    z_stream mStream;
  };
} // namespace Luna::Util
