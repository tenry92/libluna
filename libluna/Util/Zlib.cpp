#ifndef N64

#include <cstring>
#include <stdexcept>

#include <zlib.h>

#include <libluna/Util/Zlib.hpp>

#define CHUNK_SIZE 16384

using namespace Luna;
using namespace Luna::Util;

class Zlib::impl {
  public:
  void init() {
    mStream.zalloc = Z_NULL;
    mStream.zfree = Z_NULL;
    mStream.opaque = Z_NULL;
    mStream.avail_in = 0;
    mStream.next_in = Z_NULL;

    int ret = inflateInit(&mStream);

    if (ret != Z_OK) {
      throw std::runtime_error("inflateInit failed");
    }
  }

  void end() { inflateEnd(&mStream); }

  std::unique_ptr<Luna::InputStream> mReader;
  z_stream mStream;
};

Zlib::Zlib(std::unique_ptr<Luna::InputStream> reader)
    : mImpl{std::make_unique<impl>()} {
  mImpl->mReader = std::move(reader);
}

Zlib::~Zlib() = default;

std::vector<uint8_t> Zlib::inflate() {
  mImpl->mReader->seek(0);
  mImpl->init();

  unsigned char input[CHUNK_SIZE];
  unsigned char output[CHUNK_SIZE];
  int ret;

  std::vector<uint8_t> result;

  // decompress until deflate stream ends or end of file
  do {
    mImpl->mStream.avail_in =
        static_cast<unsigned int>(mImpl->mReader->read(input, 1, CHUNK_SIZE));

    if (mImpl->mStream.avail_in == 0) {
      mImpl->end();
      break;
    }

    mImpl->mStream.next_in = input;

    // run inflate() on input until output buffer not full
    do {
      mImpl->mStream.avail_out = CHUNK_SIZE;
      mImpl->mStream.next_out = output;

      ret = ::inflate(&mImpl->mStream, Z_NO_FLUSH);

      if (ret != Z_OK && ret != Z_STREAM_END) {
        mImpl->end();
        throw std::runtime_error("inflate failed");
      }

      int have = CHUNK_SIZE - mImpl->mStream.avail_out;

      result.resize(result.size() + have);
      std::memcpy(
          reinterpret_cast<void *>(result.data() + result.size() - have),
          output, have
      );
    } while (mImpl->mStream.avail_out == 0);
  } while (ret != Z_STREAM_END);

  mImpl->end();

  return result;
}

#endif
