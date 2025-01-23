#if !N64 && !NDS

#include <cstring>
#include <stdexcept>

#include <zlib.h>

#include <libluna/Util/Zlib.hpp>

#define CHUNK_SIZE 16384

using namespace Luna;
using namespace Luna::Util;

void Zlib::init() {
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

void Zlib::end() { inflateEnd(&mStream); }

Zlib::Zlib(std::unique_ptr<Luna::InputStream> reader) {
  mReader = std::move(reader);
}

Zlib::~Zlib() = default;

std::vector<uint8_t> Zlib::inflate() {
  mReader->seek(0);
  init();

  unsigned char input[CHUNK_SIZE];
  unsigned char output[CHUNK_SIZE];
  int ret;

  std::vector<uint8_t> result;

  // decompress until deflate stream ends or end of file
  do {
    mStream.avail_in =
      static_cast<unsigned int>(mReader->read(input, 1, CHUNK_SIZE));

    if (mStream.avail_in == 0) {
      end();
      break;
    }

    mStream.next_in = input;

    // run inflate() on input until output buffer not full
    do {
      mStream.avail_out = CHUNK_SIZE;
      mStream.next_out = output;

      ret = ::inflate(&mStream, Z_NO_FLUSH);

      if (ret != Z_OK && ret != Z_STREAM_END) {
        end();
        throw std::runtime_error("inflate failed");
      }

      int have = CHUNK_SIZE - mStream.avail_out;

      result.resize(result.size() + have);
      std::memcpy(
        reinterpret_cast<void *>(result.data() + result.size() - have), output,
        have
      );
    } while (mStream.avail_out == 0);
  } while (ret != Z_STREAM_END);

  end();

  return result;
}

#endif
