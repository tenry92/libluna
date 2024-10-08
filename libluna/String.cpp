#include <libluna/String.hpp>

#ifdef _WIN32
#include <windows.h> // MultiByteToWideChar
#endif

#include <cmath> // ceil
#include <codecvt> // codecvt_utf8_utf16
#include <cstring> // memcpy, strlen
#include <cwchar> // wcslen
#include <locale> // wstring_convert

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <utf8.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

using namespace Luna;
using Luna::String;

String::Iterator::Iterator(const void *data) : mData(data) {}

String::Iterator &String::Iterator::operator++() {
  CodePoint codePoint;
  mData =
      utf8codepoint(reinterpret_cast<const utf8_int8_t *>(mData), &codePoint);

  return *this;
}

String::Iterator String::Iterator::operator++(int) {
  auto retval = *this;

  CodePoint codePoint;
  mData =
      utf8codepoint(reinterpret_cast<const utf8_int8_t *>(mData), &codePoint);

  return retval;
}

bool String::Iterator::operator==(Iterator other) const {
  return mData == other.mData;
}

bool String::Iterator::operator!=(Iterator other) const {
  return mData != other.mData;
}

String::Iterator::reference String::Iterator::operator*() const {
  CodePoint codePoint;
  utf8codepoint(reinterpret_cast<const utf8_int8_t *>(mData), &codePoint);

  return codePoint;
}

String::FormatOptions String::parseFormatOptions(const String &specifier
) const {
  FormatOptions options;
  options.base = 10;
  options.uppercase = false;
  options.pad = 0;
  options.precision = 6;

  auto periodOffset = specifier.indexOf('.');

  if (periodOffset.has_value()) {
    auto padSpec = specifier.subString(0, periodOffset.value());
    auto precSpec = specifier.subString(periodOffset.value() + 1);

    if (padSpec.getLength() > 0) {
      options.pad = std::stoi(padSpec.s_str());
    }

    if (precSpec.getLength() > 0) {
      options.precision = std::stoi(precSpec.s_str());
    }
  }

  if (specifier.endsWith("x")) {
    options.base = 16;
    options.uppercase = false;
  } else if (specifier.endsWith("X")) {
    options.base = 16;
    options.uppercase = true;
  }

  return options;
}

String::String()  {
  mData.resize(1);
  mData[0] = 0;
}

String::String(const char *string) {
  if (string == nullptr) {
    mData.resize(1);
    mData[0] = 0;
    return;
  }

  auto length = strlen(string);

  if (length > 0) {
    mData.resize(length + 1);
    std::memcpy(mData.data(), string, length + 1);
  } else {
    mData.resize(1);
    mData[0] = 0;
  }
}

String::String(const char *string, std::size_t size) {
  if (string == nullptr || size == 0) {
    return;
  }

  auto length = size;

  if (length > 0) {
    mData.resize(size + 1);
    std::memcpy(mData.data(), string, length);
    mData[size] = 0;
  }
}

String::String(const wchar_t *string) {
  if (string == nullptr) {
    return;
  }

  auto length = wcslen(string); // number of characters, not bytes!

  if (length > 0) {
    mData.resize(length * sizeof(wchar_t) + 1);
    mData[0] = 0;
    auto ptr = mData.data();

    for (std::size_t i = 0; i < length; ++i) {
      auto available =
          getByteLength() - static_cast<std::size_t>(ptr - mData.data());
      ptr = reinterpret_cast<utf8_int8_t *>(
          utf8catcodepoint(ptr, string[i], available)
      );
    }

    mData.resize(utf8size(mData.data()));
  }
}

String::String(const std::string &string) {
  if (string.size() > 0) {
    mData.resize(string.size() + 1);
    std::memcpy(mData.data(), string.data(), string.size());
    mData[string.size()] = 0;
  } else {
    mData.resize(1);
    mData[0] = 0;
  }
}

String::String(const String &string)
    : String::String(string.mData.data()) {}

String::String(String &&string) {
  *this = std::move(string);
}

String::~String() {}

String String::operator=(const String &other) {
  mData = other.mData;

  return *this;
}

String String::operator=(String &&other) {
  mData = std::move(other.mData);
  other.mData.resize(1);
  other.mData[0] = 0;

  return *this;
}

String String::operator+(const String &other) const {
  if (mData.empty()) {
    return String(other);
  } else if (other.mData.empty()) {
    return String(*this);
  }

  auto length1 = strlen(mData.data());
  auto length2 = strlen(other.mData.data());
  std::vector<char> buffer(length1 + length2 + 1);
  std::memcpy(buffer.data(), mData.data(), length1 + 1);
  utf8cat(buffer.data(), other.mData.data());

  return String(buffer.data());
}

String String::operator+(String::CodePoint cp) const {
  std::vector<char> buffer(getByteLength() + 4);
  std::memcpy(buffer.data(), mData.data(), getByteLength());
  utf8catcodepoint(buffer.data() + getByteLength() - 1, cp, 5);
  buffer[buffer.size() - 1] = 0;

  return String(buffer.data());
}

bool String::operator==(const String &other) const {
  if (getLength() == 0 && other.getLength() == 0) {
    return true;
  }

  if (getLength() == 0 || other.getLength() == 0) {
    return false;
  }

  return utf8cmp(mData.data(), other.mData.data()) == 0;
}

bool String::operator<(const String &other) const {
  return utf8cmp(mData.data(), other.mData.data()) < 0;
}

bool String::operator>(const String &other) const {
  return utf8cmp(mData.data(), other.mData.data()) > 0;
}

String::CodePoint String::operator[](std::size_t index) const {
  if (mData.empty()) {
    // should not be possible since it shall always contain a null terminator
    return 0;
  }

  CodePoint codePoint = 0;
  auto ptr = mData.data();

  for (std::size_t offset = 0; offset <= index; ++offset) {
    ptr = utf8codepoint(ptr, &codePoint);
  }

  return codePoint;
}

std::size_t String::getLength() const {
  if (mData.empty()) {
    return 0;
  }

  return utf8nlen(mData.data(), mData.size());
}

std::size_t String::getByteLength() const { return mData.size(); }

bool String::isEmpty() const {
  return mData.empty() || utf8len(mData.data()) == 0;
}

String String::subString(std::size_t start) const {
  auto ptr = mData.data();
  CodePoint codePoint;

  if (start >= getLength()) {
    return String();
  }

  for (std::size_t i = 0; i < start; ++i) {
    ptr = utf8codepoint(ptr, &codePoint);
  }

  return String(reinterpret_cast<const char *>(ptr));
}

String String::subString(std::size_t start, std::size_t end) const {
  auto ptr = mData.data();
  CodePoint codePoint;

  if (start > getLength()) {
    start = getLength();
  }

  if (end > getLength()) {
    end = getLength();
  } else if (end < start) {
    end = start;
  }

  std::size_t length = end - start;

  for (std::size_t i = 0; i < start; ++i) {
    ptr = utf8codepoint(ptr, &codePoint);
  }

  auto endPtr = ptr;

  for (std::size_t i = 0; i < length; ++i) {
    endPtr = utf8codepoint(endPtr, &codePoint);
  }

  std::size_t lengthBytes = static_cast<std::size_t>(
      reinterpret_cast<const char *>(endPtr) -
      reinterpret_cast<const char *>(ptr)
  );

  return String(reinterpret_cast<const char *>(ptr), lengthBytes);
}

String String::replace(const String &search, const String &replacement) const {
  auto offset = indexOf(search);

  if (!offset.has_value()) {
    return *this;
  }

  return subString(0, offset.value()) + replacement +
         subString(offset.value() + search.getLength());
}

String
String::replaceAll(const String &search, const String &replacement) const {
  String output;
  auto offset = indexOf(search);
  std::size_t lastOffset = 0;

  while (offset.has_value()) {
    output = output + subString(lastOffset, offset.value()) + replacement;
    lastOffset = offset.value() + search.getLength();
    offset = indexOf(search, lastOffset);
  }

  output = output + subString(lastOffset);

  return output;
}

std::optional<std::size_t>
String::indexOf(CodePoint codePoint, std::size_t fromIndex) const {
  if (mData.empty()) {
    return std::optional<std::size_t>();
  }

  if (fromIndex > 0) {
    auto result = subString(fromIndex).indexOf(codePoint);

    if (result.has_value()) {
      return result.value() + fromIndex;
    }

    return result;
  }

  auto ptr = utf8chr(mData.data(), codePoint);

  if (ptr == nullptr) {
    return std::optional<std::size_t>();
  }

  size_t index = 0;
  auto subPtr = mData.data();

  while (subPtr < ptr) {
    ++subPtr;
    ++index;
  }

  return index;
}

std::optional<std::size_t>
String::indexOf(const String &string, std::size_t fromIndex) const {
  if (mData.empty() || string.mData.empty()) {
    return std::optional<std::size_t>();
  }

  if (fromIndex > 0) {
    auto result = subString(fromIndex).indexOf(string);

    if (result.has_value()) {
      return result.value() + fromIndex;
    }

    return result;
  }

  auto ptr = utf8str(mData.data(), string.mData.data());

  if (ptr == nullptr) {
    return std::optional<std::size_t>();
  }

  size_t index = 0;
  auto subPtr = mData.data();

  while (subPtr < ptr) {
    ++subPtr;
    ++index;
  }

  return index;
}

String String::repeat(std::size_t count) const {
  if (mData.empty() || count == 0) {
    return String();
  }

  auto rawLength = utf8size(mData.data()) - 1;
  auto totalLength = rawLength * count;
  std::vector<char> buffer(totalLength + 1);

  for (std::size_t i = 0; i < count; ++i) {
    auto offset = i * rawLength;
    std::memcpy(&buffer.data()[offset], mData.data(), rawLength);
  }

  buffer.data()[totalLength] = 0;

  return String(buffer.data());
}

String String::toLowerCase() const {
  String output(mData.data());
  utf8lwr(output.mData.data());

  return output;
}

String String::toUpperCase() const {
  String output(mData.data());
  utf8upr(output.mData.data());

  return output;
}

String String::padStart(std::size_t targetLength, const String &string) const {
  auto padLength = targetLength - getLength();

  if (padLength <= 0 || string.isEmpty()) {
    return *this;
  }

  /// @todo Can we do this without using floats?
  auto padRepeat = static_cast<size_t>(std::ceil(
      static_cast<float>(padLength) / static_cast<float>(string.getLength())
  ));

  auto padString = string.repeat(padRepeat);

  return padString.subString(0, padLength) + *this;
}

String String::padEnd(std::size_t targetLength, const String &string) const {
  auto padLength = targetLength - getLength();

  if (padLength <= 0 || string.isEmpty()) {
    return *this;
  }

  /// @todo Can we do this without using floats?
  auto padRepeat = static_cast<size_t>(std::ceil(
      static_cast<float>(padLength) / static_cast<float>(string.getLength())
  ));

  auto padString = string.repeat(padRepeat);

  return (*this) + padString.subString(0, padLength);
}

bool String::startsWith(const String &other) const {
  return subString(0, other.getLength()) == other;
}

bool String::endsWith(const String &other) const {
  return subString(getLength() - other.getLength(), getLength()) == other;
}

const char *String::c_str() const { return mData.data(); }

std::string String::s_str() const {
  return std::string(c_str(), getByteLength() - 1);
}

std::wstring String::w_str() const {
#ifdef _WIN32
  std::wstring output;
  int convertResult = MultiByteToWideChar(CP_UTF8, 0, c_str(), -1, nullptr, 0);

  if (convertResult <= 0) {
    throw new std::runtime_error("unable to convert to wide string");
  }

  output.resize(convertResult + 10);
  convertResult = MultiByteToWideChar(
      CP_UTF8, 0, c_str(), -1, &output[0], static_cast<int>(output.size())
  );
  return output;
#else
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

  return converter.from_bytes(s_str());
#endif
}

String::Iterator String::begin() { return Iterator(mData.data()); }

String::Iterator String::end() {
  return Iterator(reinterpret_cast<void *>(
      mData.data() + utf8size(mData.data()) - 1
  ));
}

String::Iterator String::cbegin() const {
  return Iterator(mData.data());
}

String::Iterator String::cend() const {
  return Iterator(reinterpret_cast<const void *>(
      mData.data() + utf8size(mData.data()) - 1
  ));
}

String Literals::operator"" _s(const char *string, size_t length) {
  return String(string, length);
}
