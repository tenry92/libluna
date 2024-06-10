/**
 * \file String.hpp
 *
 * \brief String
 */

#pragma once

#include <stdint.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace Luna {
  /**
   * \brief UTF-8 encoded text string.
   */
  class String {
    public:
    /**
     * \brief This type represents a single unicode code point.
     */
    using CodePoint = int32_t;

    using OptionalIndex = std::optional<std::size_t>;

    /**
     * \brief Allows for iterating the strings.
     *
     * \par Example
     * \code{.cpp}
     * String string("foobar");
     *
     * for(const auto &&codePoint : string) {
     *   std::cout << codePoint << std::endl;
     * }
     *
     * // output:
     * // 102
     * // 111
     * // 111
     * // 98
     * // 97
     * // 114
     * \endcode
     */
    class Iterator {
      public:
      using iterator_category = std::input_iterator_tag;
      using value_type = CodePoint;
      using difference_type = CodePoint;
      using pointer = CodePoint *;
      using reference = CodePoint;

      explicit Iterator(const void *data);
      Iterator &operator++();
      Iterator operator++(int);
      bool operator==(Iterator other) const;
      bool operator!=(Iterator other) const;
      reference operator*() const;

      private:
      const void *mData;
    };

    private:
    struct FormatOptions {
      int base;
      bool uppercase;
      int pad;
      int precision;
    };

    FormatOptions parseFormatOptions(const String &specifier) const;

    public:
    /**
     * \brief Construct an empty string.
     */
    String();

    /**
     * \brief Construct string from C string.
     *
     * \param string UTF-8 encoded string.
     */
    String(const char *string);

    /**
     * \brief Construct string from C string including NUL values.
     *
     * \param string UTF-8 encoded string.
     * \param size Size of \p string in bytes, excluding possibly final null
     * terminator.
     *
     * \par Example
     * \code{.cpp}
     * auto string = String("foo\0bar", 7);
     * \endcode
     */
    String(const char *string, std::size_t size);

    /**
     * \brief Construct string from wide C string.
     *
     * \par Example
     * \code{.cpp}
     * auto string = String(L"wide string");
     * \endcode
     */
    String(const wchar_t *string);

    /**
     * \brief Construct string from C++ string.
     */
    String(const std::string &string);

    /**
     * \brief Copy constructor.
     */
    String(const String &string);

    /**
     * \brief Move constructor.
     */
    String(String &&string);

    ~String();

    template <
        typename Type,
        typename std::enable_if_t<std::is_integral<Type>::value, bool> = true>
    static String fromInteger(Type number, int base = 10) {
      if (base < 2 || base > 36) {
        throw std::invalid_argument("base must be between 2 and 36 inclusive");
      }

      std::vector<char> buffer;

      bool negative = number < 0;

      if constexpr (std::is_signed_v<Type>) {
        if (negative) {
          number = -static_cast<Type>(number);
        }
      }

      if (number == 0) {
        buffer.push_back('0');
      }

      while (number > 0) {
        auto digitValue = static_cast<char>(number % base);

        if (digitValue <= 9) {
          buffer.push_back(static_cast<char>('0' + digitValue));
        } else {
          buffer.push_back(static_cast<char>('a' + digitValue - 10));
        }

        number -= digitValue;
        number /= base;
      }

      if (negative) {
        buffer.push_back('-');
      }

      std::reverse(buffer.begin(), buffer.end());

      return String(buffer.data(), buffer.size());
    }

    template <
        typename Type, typename std::enable_if_t<
                           std::is_floating_point<Type>::value, bool> = true>
    static String fromFloat(Type number, int precision = 6, int base = 10) {
      if (precision < 0) {
        throw std::invalid_argument("precision must be positive");
      }

      if (base < 2 || base > 36) {
        throw std::invalid_argument("base must be between 2 and 36 inclusive");
      }

      int intNumber = static_cast<int>(number);
      Type fraction = std::abs(number) - static_cast<Type>(std::abs(intNumber));

      auto intString = fromInteger(intNumber, base);

      if (precision == 0) {
        return intString;
      }

      std::vector<char> buffer;

      for (int position = 0; position < precision; ++position) {
        fraction = fraction * static_cast<Type>(base);

        char digitValue = static_cast<char>(fraction);
        fraction -= digitValue;

        if (digitValue <= 9) {
          buffer.push_back(static_cast<char>('0' + digitValue));
        } else {
          buffer.push_back(static_cast<char>('a' + digitValue - 10));
        }
      }

      return intString + '.' + String(buffer.data(), buffer.size());
    }

    String operator=(const String &other);

    String operator=(const String &&other);

    /**
     * \brief Concatenate two strings.
     */
    String operator+(const String &other) const;

    String operator+(CodePoint cp) const;

    /**
     * \brief Check if two strings are equal.
     */
    bool operator==(const String &other) const;

    /**
     * \brief Check if two strings are not equal.
     */
    inline bool operator!=(const String &other) const {
      return !(*this == other);
    }

    bool operator<(const String &other) const;

    bool operator>(const String &other) const;

    /**
     * \brief Get code point at \p index .
     *
     * \return Code point at \p index or `0` if out of range.
     */
    CodePoint operator[](std::size_t index) const;

    /**
     * \brief Length of string in UTF-8 code points.
     */
    std::size_t getLength() const;

    /**
     * \brief Length of string in raw bytes.
     */
    std::size_t getByteLength() const;

    bool isEmpty() const;

    /**
     * \brief Returns sub string.
     *
     * \param start 0-based start index.
     */
    String subString(std::size_t start) const;

    /**
     * \brief Returns sub string.
     *
     * \param start 0-based start index.
     * \param end 0-based ending index (excluded).
     *
     * \par Example
     * \code{.cpp}
     * auto subString = String("abcdef").subString(2, 4);
     * assert(subString == "cd");
     * \endcode
     */
    String subString(std::size_t start, std::size_t end) const;

    String replace(const String &search, const String &replacement) const;

    String replaceAll(const String &search, const String &replacement) const;

    /**
     * \brief Find index of code point.
     *
     * \param codePoint Code point to search for.
     * \param fromIndex Index to start searching from.
     *
     * \par Example
     * \code
     * String string("abbacc");
     *
     * auto index1 = string.indexOf('a');
     * auto index2 = string.indexOf('a', 1);
     * auto index3 = string.indexOf('a', 10);
     *
     * assert(index1.has_value() && index1.value() == 0);
     * assert(index2             && index2         == 3);
     * assert(!index3.has_value());
     *
     * \endcode
     */
    OptionalIndex indexOf(CodePoint codePoint, std::size_t fromIndex = 0) const;

    /**
     * \brief Find index of sub string.
     *
     * \param string Sub string to search for.
     * \param fromIndex Index to start searching from.
     *
     * \par Example
     * \code
     * String string("lorem ipsum lorem ipsum");
     *
     * auto index1 = string.indexOf("ipsum");
     * auto index2 = string.indexOf("ipsum", 10);
     * auto index3 = string.indexOf("ipsum", 100);
     *
     * assert(index1.has_value() && index1.value() == 6);
     * assert(index2             && index2         == 18);
     * assert(!index3.has_value());
     *
     * \endcode
     */
    OptionalIndex
    indexOf(const String &string, std::size_t fromIndex = 0) const;

    // OptionalIndex indexOf(const std::regex &regex, std::size_t fromIndex = 0)
    // const;

    /**
     * \brief Split string by code point.
     *
     * \par Example
     * \code{.cpp}
     * std::list<String> parts;
     * String("path/to/file").split('/', std::back_inserter(parts));
     *
     * // parts == {"path", "to", "file"}
     * \endcode
     */
    template <class OutputIt>
    void split(CodePoint codePoint, OutputIt output) const {
      std::size_t lastOffset = 0;
      auto offset = indexOf(codePoint);

      while (offset.has_value()) {
        *output = subString(lastOffset, offset.value());
        ++output;
        lastOffset = offset.value() + 1;
        offset = indexOf(codePoint, lastOffset);
      }

      *output = subString(lastOffset);
    }

    /**
     * \brief Split string by sub string.
     *
     * \par Example
     * \code{.cpp}
     * std::list<String> parts;
     * String("path/to/file").split("/", std::back_inserter(parts));
     *
     * // parts == {"path", "to", "file"}
     * \endcode
     */
    template <class OutputIt>
    void split(const String &string, OutputIt output) const {
      std::size_t lastOffset = 0;
      auto offset = indexOf(string);

      while (offset.has_value()) {
        *output = subString(lastOffset, offset.value());
        ++output;
        lastOffset = offset.value() + string.getLength();
        offset = indexOf(string, lastOffset);
      }

      *output = subString(lastOffset);
    }

    std::list<String> split(CodePoint codePoint) {
      std::list<String> parts;
      split(codePoint, std::back_inserter(parts));
      return parts;
    }

    std::list<String> split(const String &string) {
      std::list<String> parts;
      split(string, std::back_inserter(parts));
      return parts;
    }

    template <class InputIt>
    static String join(const String &separator, InputIt first, InputIt last) {
      String output = "";
      bool initial = true;

      while (first != last) {
        if (!initial) {
          output = output + separator;
        } else {
          initial = false;
        }

        output = output + *first++;
      }

      return output;
    }

    /**
     * \brief Repeat string.
     *
     * \param count Number of repeats.
     * \return String repeated \p count times.
     *
     * \par Example
     * \code{.cpp}
     * auto string = String("foo").repeat(3);
     * assert(string == "foofoofoo");
     * \endcode
     */
    String repeat(std::size_t count) const;

    String toLowerCase() const;

    String toUpperCase() const;

    /**
     * \brief Pad start of string.
     *
     * \param targetLength The target (mininum) string length.
     * \param string The pad string.
     *
     * \par Example
     * \code{.cpp}
     * auto str1 = String("19").padStart(4, "0");
     * auto str2 = String("11").padStart(5, "ab");
     *
     * assert(str1 == "0019");
     * assert(str2 == "aba19");
     * \endcode
     */
    String padStart(std::size_t targetLength, const String &string) const;

    /**
     * \brief Pad end of string.
     *
     * \param targetLength The target (mininum) string length.
     * \param string The pad string.
     *
     * \par Example
     * \code{.cpp}
     * auto str1 = String("19").padEnd(4, "0");
     * auto str2 = String("11").padEnd(5, "ab");
     *
     * assert(str1 == "1900");
     * assert(str2 == "19aba");
     * \endcode
     */
    String padEnd(std::size_t targetLength, const String &string) const;

    /**
     * \brief Check if string starts with a sub string.
     */
    bool startsWith(const String &other) const;

    /**
     * \brief Check if string ends with a sub string.
     */
    bool endsWith(const String &other) const;

    /**
     * \brief Get null-terminated C string.
     *
     * The returned value is only valid as long as the String object exists.
     */
    const char *c_str() const;

    /**
     * \brief Get standard C++ string.
     */
    std::string s_str() const;

    /**
     * \brief Get standard C++ wide string.
     */
    std::wstring w_str() const;

    Iterator begin();
    Iterator end();
    Iterator cbegin() const;
    Iterator cend() const;
    inline Iterator begin() const { return cbegin(); }
    inline Iterator end() const { return cend(); }

    /**
     * \brief Replaces placeholders by values.
     *
     * Placeholders indicated by \c "{}" are replaced by the parameters.
     * To produce a literal \c "{" char in the output, write \c "{{" instead.
     *
     * \par Example
     * \code{.cpp}
     * auto formatted1 = String("the value of {} is {}")  .format("myvar", 42);
     * auto formatted2 = String("pos = {{ x: {}, y: {} }").format(19, 11);
     *
     * assert(formatted1 == "the value of myvar is 42");
     * assert(formatted2 == "pos = { x: 19, y: 11 }");
     * \endcode
     */
    String format() const { return *this; }

    String
    formatCallback(std::function<String(FormatOptions, const String &)> callback
    ) const {
      auto nextOffset = indexOf('{');
      auto prepend = subString(0, nextOffset.value_or(getLength()));

      if (nextOffset.has_value()) {
        // {-sign found
        /// @todo implement += operator
        String specifier = "";
        String replacement = "";

        while (true) {
          if (nextOffset.value() >= this->getLength() - 1) {
            // end of string
            break;
          }

          auto ch = (*this)[++nextOffset.value()];

          if (ch == '{' && specifier.isEmpty()) {
            // literal '{' character
            auto offset = nextOffset.value();
            nextOffset = indexOf('{', ++nextOffset.value());
            prepend = subString(0, offset) +
                      subString(offset + 1, nextOffset.value_or(getLength()));
            continue;
          }

          if (ch == '}') {
            auto append = subString(++nextOffset.value());
            replacement = callback(parseFormatOptions(specifier), append);
            break;
          }

          specifier = specifier + ch;
        }

        return prepend + replacement;
      }

      // no (more) {-sign found; prepend = whole string
      return prepend;
    }

    // template<typename ValueType, typename... ArgTypes>
    // String format(ValueType value, ArgTypes... args) const {
    //   return formatCallback([&](const String &specifier, const String
    //   &append) {
    //     return String(std::to_string(value)) + append.format(args...);
    //   });
    // }

    template <typename... ArgTypes>
    String format(int value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions options, const String &append) {
        return String(fromInteger(value, options.base)) +
               append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(long value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions options, const String &append) {
        return String(fromInteger(value, options.base)) +
               append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(long long value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions options, const String &append) {
        return String(fromInteger(value, options.base)) +
               append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(unsigned value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions options, const String &append) {
        return String(fromInteger(value, options.base)) +
               append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(unsigned long value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions options, const String &append) {
        return String(fromInteger(value, options.base)) +
               append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(unsigned long long value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions options, const String &append) {
        return String(fromInteger(value, options.base)) +
               append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(float value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions options, const String &append) {
        return String(fromFloat(value, options.precision, options.base)) +
               append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(double value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions options, const String &append) {
        return String(fromFloat(value, options.precision, options.base)) +
               append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(long double value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions options, const String &append) {
        return String(fromFloat(value, options.precision, options.base)) +
               append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(const String &value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions, const String &append) {
        return value + append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(const char *value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions, const String &append) {
        return String(value) + append.format(args...);
      });
    }

    template <typename... ArgTypes>
    String format(void *value, ArgTypes... args) const {
      return formatCallback([&](FormatOptions, const String &append) {
        return String(fromInteger(reinterpret_cast<unsigned long>(value), 16)
                          .padStart(16, "0")) +
               append.format(args...);
      });
    }

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };

  namespace Literals {
    String operator"" _s(const char *string, size_t length);
  }
} // namespace Luna
