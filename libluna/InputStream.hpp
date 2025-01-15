#pragma once

#include <cstdint>

namespace Luna {
  /**
   * @defgroup streams Streams
   *
   * @brief Streams for reading and writing data.
   *
   * Example:
   *
   * @code{.cpp}
   * void readData(InputStream &input) {
   *   auto fileSize = input.getSize();
   *
   *   uint32_t value;
   *   input.read(&value);
   *
   *   uint16_t values[4];
   *   input.read(values, 4);
   *
   *   auto newPosition = input.skip(8);
   *   newPosition = input.seekRelative(-4);
   *   newPosition = input.tell();
   *
   *   while (!input.eof()) {
   *     uint8_t byte;
   *     input.read(&byte);
   *   }
   * }
   * @endcode
   */
  /**
   * @ingroup streams
   *
   * @brief Abstract binary input stream.
   */
  class InputStream {
    public:
    inline virtual ~InputStream() = default;

    virtual bool isValid() const = 0;

    /**
     * @brief Check if file is valid and exists.
     */
    inline operator bool() const { return isValid(); }

    /**
     * @brief Get file size in bytes.
     */
    virtual std::size_t getSize() const = 0;

    /**
     * @brief Check if end of file was reached.
     */
    virtual bool eof() const = 0;

    /**
     * @brief Get the current position in the file.
     */
    virtual std::size_t tell() = 0;

    /**
     * @brief Seek to a position in the file.
     *
     * @return The actual position seeked to.
     */
    virtual std::size_t seek(std::size_t position) = 0;

    /**
     * @brief Seek relative to current position in the file.
     *
     * @return The new absolute position in the file.
     */
    virtual std::size_t seekRelative(int relativePosition) = 0;

    /**
     * @brief Skip specified amount of bytes.
     *
     * @return The new absolute position in the file.
     */
    inline std::size_t skip(int length) { return seekRelative(length); }

    /**
     * @brief Read data.
     *
     * @param buffer Destination to write into. Must be able to receive at least
     * @p objectSize * @p objectCount bytes.
     * @param objectSize Size of a single entry in bytes.
     * @param objectCount Number of objects to read.
     * @return Actual number of objects read.
     *
     * @par Example
     * @code{.cpp}
     * uint8_t byte;
     * myFile.read(&byte, 1, 1);
     *
     * uint8_t moreBytes[4];
     * myFile.read(moreBytes, sizeof(uint8_t), 4);
     * @endcode
     */
    virtual std::size_t
    read(uint8_t *buffer, std::size_t objectSize, std::size_t objectCount) = 0;

    /**
     * @brief Read data.
     *
     * @param buffer Destination to write into. Must be able to receive at least
     * @p objectSize * @p objectCount bytes.
     * @param objectSize Size of a single entry in bytes.
     * @param objectCount Number of objects to read.
     * @return Actual number of objects read.
     *
     * @par Example
     * @code{.cpp}
     * uint16_t value;
     * myFile.read(&value, 2, 1);
     *
     * uint16_t values[9];
     * myFile.read(values, sizeof(uint16_t), 9);
     *
     * uint32_t moreValues[4];
     * myFile.read(moreValues, sizeof(uint32_t), 4);
     * // alternative:
     * myFile.read(moreValues, sizeof(uint32_t), sizeof(moreValues) /
     * sizeof(uint32_t)); @endcode
     */
    template <typename BufferType>
    std::size_t
    read(BufferType *buffer, std::size_t objectSize, std::size_t objectCount) {
      return read(reinterpret_cast<uint8_t *>(buffer), objectSize, objectCount);
    }

    /**
     * @brief Read specified amount of bytes.
     *
     * @param buffer Destination to write into. Must be able to receive at least
     * @p byteCount bytes.
     * @param byteCount Number of bytes to read.
     *
     * @par Example
     * @code{.cpp}
     * uint8_t byte;
     * myFile.read(&byte);
     *
     * uint8_t moreBytes[4];
     * myFile.read(moreBytes, 4);
     * @endcode
     */
    inline std::size_t read(uint8_t *buffer, std::size_t byteCount = 1) {
      return read(buffer, 1, byteCount);
    }

    /**
     * @brief Read specified amount of objects.
     *
     * @param buffer Destination to write into. Must be able to receive at least
     * <tt>sizeof(BufferType) * byteCount</tt> bytes.
     * @param objectCount Number of objects to read.
     *
     * @par Example
     * @code{.cpp}
     * uint16_t value;
     * myFile.read(&value);
     *
     * uint32_t moreValues[6];
     * myFile.read(moreValues, 6);
     * @endcode
     */
    template <typename BufferType>
    std::size_t read(BufferType *buffer, std::size_t objectCount = 1) {
      return read(
          reinterpret_cast<uint8_t *>(buffer), sizeof(BufferType), objectCount
      );
    }

    bool readLine(char *buffer, std::size_t bufferSize) {
      if (bufferSize == 0) {
        return false;
      }

      for (std::size_t i = 0; i < bufferSize; i++) {
        if (read(&buffer[i]) == 0) {
          // end of file reached
          buffer[i] = '\0';

          if (i == 0) {
            return false;
          }

          break;
        }

        if (buffer[i] == '\n') {
          // end of line reached
          buffer[i] = '\0';
          break;
        }
      }

      return true;
    }
  };
} // namespace Luna
