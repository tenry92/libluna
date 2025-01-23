/**
 * @file String.hpp
 *
 * @brief String
 */

#pragma once

#include <stdint.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <utf8.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <list>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Luna {
  /**
   * @brief UTF-8 encoded text string.
   */
  class String {
    public:
    using CodePoint = int32_t;

    using OptionalIndex = std::optional<std::size_t>;

    class Iterator {
      public:
      Iterator(const String &string, std::size_t index);

      CodePoint operator*() const;

      Iterator &operator++();
      Iterator operator++(int);

      bool operator==(const Iterator &other) const;
      bool operator!=(const Iterator &other) const;

      private:
      const String &mString;
      std::size_t mIndex;
    };

    /**
     * @brief Empty string constructor.
     */
    String();

    /**
     * @brief Copy string constructor.
     */
    String(char *other);

    /**
     * @brief Copy string constructor with length in bytes.
     */
    String(char *other, std::size_t length);

    /**
     * @brief Const string view constructor.
     */
    String(const char *other);

    /**
     * @brief Const string view constructor with length in bytes.
     * 
     * If the other string does not terminate with a NULL character at the given
     * length, the data is copied.
     */
    String(const char *other, std::size_t length);

    /**
     * @brief Copy wide string constructor.
     */
    String(const wchar_t *other);

    String(const std::string &other);

    String(const String &other);

    String(CodePoint codePoint);

    String(String &&other);

    ~String();

    String &operator=(char *other);
    String &operator=(const char *other);
    String &operator=(const String &other);
    String &operator=(String &&other);

    bool operator==(const String &other) const;

    inline bool operator!=(const String &other) const {
      return !(*this == other);
    }

    bool operator<(const String &other) const;

    bool operator>(const String &other) const;

    String operator+(CodePoint cp) const;

    String operator+(const String &other) const;

    /**
     * @brief Get the length of the string in code points.
     */
    std::size_t getLength() const;

    /**
     * @brief Get the length of the string in bytes (excluding NULL terminator).
     */
    std::size_t getByteLength() const;

    bool isEmpty() const;

    bool startsWith(const String &other) const;

    bool endsWith(const String &other) const;

    const char *c_str() const;

    const utf8_int8_t *data() const;

    CodePoint operator[](std::size_t index) const;

    String subString(std::size_t start) const;

    String subString(std::size_t start, std::size_t length) const;

    OptionalIndex indexOf(CodePoint codePoint, std::size_t fromIndex = 0) const;

    OptionalIndex indexOf(const String &string, std::size_t fromIndex = 0) const;

    String replace(const String &search, const String &replacement) const;

    String replaceAll(const String &search, const String &replacement) const;

    void clear();

    template <typename OutputIt>
    void split(CodePoint delimiter, OutputIt output) const {
      if (isEmpty()) {
        return;
      }

      std::size_t fromIndex = 0;

      while (true) {
        auto optionalIndex = indexOf(delimiter, fromIndex);

        if (!optionalIndex.has_value()) {
          *output++ = subString(fromIndex);
          break;
        }

        auto delimiterIndex = optionalIndex.value();
        *output++ = subString(fromIndex, delimiterIndex - fromIndex);
        fromIndex = delimiterIndex + 1;
      }
    }

    template <typename OutputIt>
    void split(const String &delimiter, OutputIt output) const {
      if (isEmpty()) {
        return;
      }

      std::size_t fromIndex = 0;

      while (true) {
        auto optionalIndex = indexOf(delimiter, fromIndex);

        if (!optionalIndex.has_value()) {
          *output++ = subString(fromIndex);
          break;
        }

        auto delimiterIndex = optionalIndex.value();
        *output++ = subString(fromIndex, delimiterIndex - fromIndex);
        fromIndex = delimiterIndex + delimiter.getByteLength();
      }
    }

    std::list<String> split(CodePoint delimiter) const;

    std::list<String> split(const String &delimiter) const;

    template <typename InputIt>
    static String join(InputIt begin, InputIt end, CodePoint delimiter) {
      String result;

      if (begin == end) {
        return result;
      }

      result = *begin++;

      while (begin != end) {
        result = result + delimiter + *begin++;
      }

      return result;
    }

    template <typename InputIt>
    static String join(InputIt begin, InputIt end, const String &delimiter) {
      String result;

      if (begin == end) {
        return result;
      }

      result = *begin++;

      while (begin != end) {
        result = result + delimiter + *begin++;
      }

      return result;
    }

    Iterator begin() const;
    Iterator end() const;

    private:
    using StringData = std::vector<utf8_int8_t>;

    /**
     * @brief Pointer to non-owning C-string or shared pointer to UTF-8 encoded
     * string.
     *
     * The data is implicitly shared between instances. When an instance is
     * modified, the data is copied.
     */
    std::variant<const char *, std::shared_ptr<StringData>> mString;

    /**
     * @brief Length of the string view in bytes if referencing non-owning
     * C-string (excluding potential NULL terminator).
     */
    std::size_t mStringViewByteLength;
  };
} // namespace Luna
