#include <libluna/String.hpp>

#ifdef _WIN32
#include <windows.h> // MultiByteToWideChar
#endif

#include <cmath>   // ceil
#include <codecvt> // codecvt_utf8_utf16
#include <cstring> // memcpy, strlen
#include <cwchar>  // wcslen
#include <locale>  // wstring_convert

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <utf8.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace {
  const char nullString[] = "";
}

namespace Luna {
  String::Iterator::Iterator(const String &string, std::size_t index)
      : mString(string), mIndex(index) {}

  String::CodePoint String::Iterator::operator*() const {
    return mString[mIndex];
  }

  String::Iterator &String::Iterator::operator++() {
    ++mIndex;
    return *this;
  }

  String::Iterator String::Iterator::operator++(int) {
    Iterator copy = *this;
    ++mIndex;
    return copy;
  }

  bool String::Iterator::operator==(const Iterator &other) const {
    return mIndex == other.mIndex;
  }

  bool String::Iterator::operator!=(const Iterator &other) const {
    return mIndex != other.mIndex;
  }

  String::String() { clear(); }

  String::String(char *other) : String() {
    if (other == nullptr) {
      return;
    }

    auto length = std::strlen(other);

    if (length == 0) {
      return;
    }

    auto stringData = std::make_shared<StringData>();
    stringData->resize(length + 1);
    std::memcpy(stringData->data(), other, length);
    stringData->data()[length] = '\0';

    mString = stringData;
  }

  String::String(char *other, std::size_t length) : String() {
    if (other == nullptr || length == 0) {
      return;
    }

    auto stringData = std::make_shared<StringData>();
    stringData->resize(length + 1);
    std::memcpy(stringData->data(), other, length);
    stringData->data()[length] = '\0';

    mString = stringData;
  }

  String::String(const char *other) : String() {
    if (other == nullptr) {
      return;
    }

    auto length = std::strlen(other);

    if (length == 0) {
      return;
    }

    if (other[length] != '\0') {
      auto stringData = std::make_shared<StringData>();
      stringData->resize(length + 1);
      std::memcpy(stringData->data(), other, length);
      stringData->data()[length] = '\0';

      mString = stringData;

      return;
    }

    mString = other;
    mStringViewByteLength = length;
  }

  String::String(const char *other, std::size_t length) : String() {
    if (other == nullptr || length == 0) {
      return;
    }

    if (other[length] != '\0') {
      auto stringData = std::make_shared<StringData>();
      stringData->resize(length + 1);
      std::memcpy(stringData->data(), other, length);
      stringData->data()[length] = '\0';

      mString = stringData;

      return;
    }

    mString = other;
    mStringViewByteLength = length;
  }

  String::String(const wchar_t *other) : String() {
    if (other == nullptr) {
      return;
    }

    auto length = std::wcslen(other); // number of characters, not bytes!

    if (length == 0) {
      return;
    }

    auto stringData = std::make_shared<StringData>();
    stringData->resize(length * sizeof(wchar_t) + 1);
    stringData->at(0) = '\0';

    auto ptr = stringData->data();

    for (std::size_t i = 0; i < length; ++i) {
      auto available =
        stringData->size() - 1 - static_cast<std::size_t>(ptr - data());
      ptr = reinterpret_cast<utf8_int8_t *>(
        utf8catcodepoint(ptr, other[i], available)
      );
    }

    *ptr = '\0';

    mString = stringData;
  }

  String::String(const std::string &other) : String() {
    if (other.empty()) {
      return;
    }

    auto stringData = std::make_shared<StringData>();
    stringData->resize(other.size() + 1);
    std::memcpy(stringData->data(), other.data(), other.size());
    stringData->data()[other.size()] = '\0';

    mString = stringData;
  }

  String::String(const String &other) = default;

  String::String(CodePoint codePoint) : String() {
    if (codePoint == 0) {
      return;
    }

    auto stringData = std::make_shared<StringData>();
    stringData->resize(5);
    stringData->data()[0] = '\0';
    auto end = utf8catcodepoint(stringData->data(), codePoint, 5);
    *end = '\0';

    stringData->resize(end - stringData->data() + 1);

    mString = stringData;
  }

  String::String(String &&other) {
    mString = other.mString;
    mStringViewByteLength = other.mStringViewByteLength;

    other.mString = nullString;
    other.mStringViewByteLength = 0;
  }

  String::~String() = default;

  String &String::operator=(char *other) {
    clear();

    if (other == nullptr) {
      return *this;
    }

    auto length = std::strlen(other);

    if (length == 0) {
      return *this;
    }

    auto stringData = std::make_shared<StringData>();
    stringData->resize(length + 1);
    std::memcpy(stringData->data(), other, length);
    stringData->data()[length] = '\0';

    mString = stringData;

    return *this;
  }

  String &String::operator=(const char *other) {
    clear();

    if (other == nullptr) {
      return *this;
    }

    auto length = std::strlen(other);

    if (length == 0) {
      return *this;
    }

    if (other[length] != '\0') {
      auto stringData = std::make_shared<StringData>();
      stringData->resize(length + 1);
      std::memcpy(stringData->data(), other, length);
      stringData->data()[length] = '\0';

      mString = stringData;

      return *this;
    }

    mString = other;
    mStringViewByteLength = length;

    return *this;
  }

  String &String::operator=(const String &other) {
    mString = other.mString;
    mStringViewByteLength = other.mStringViewByteLength;

    return *this;
  }

  String &String::operator=(String &&other) {
    mString = other.mString;
    mStringViewByteLength = other.mStringViewByteLength;

    if (std::holds_alternative<const char *>(other.mString)) {
      other.mString = nullString;
      other.mStringViewByteLength = 0;
    } else {
      other.mString = std::shared_ptr<StringData>();
    }

    return *this;
  }

  bool String::operator==(const String &other) const {
    if (getLength() == 0 && other.getLength() == 0) {
      return true;
    }

    if (getLength() != other.getLength()) {
      return false;
    }

    if (std::holds_alternative<const char *>(mString)) {
      auto stringView = std::get<const char *>(mString);

      if (std::holds_alternative<const char *>(other.mString)) {
        // we can quit early if the pointers are equal
        auto otherStringView = std::get<const char *>(other.mString);

        if (stringView == otherStringView) {
          return true;
        }
      }
    } else {
      auto stringData = std::get<std::shared_ptr<StringData>>(mString);

      if (std::holds_alternative<std::shared_ptr<StringData>>(other.mString)) {
        // wen can quit early if the pointers are equal
        auto otherStringData =
          std::get<std::shared_ptr<StringData>>(other.mString);

        if (stringData == otherStringData) {
          return true;
        }
      }
    }

    // in all other cases, we need to compare the actual data
    return utf8cmp(data(), other.data()) == 0;
  }

  bool String::operator<(const String &other) const {
    return utf8cmp(data(), other.data()) < 0;
  }

  bool String::operator>(const String &other) const {
    return utf8cmp(data(), other.data()) > 0;
  }

  String String::operator+(CodePoint cp) const {
    if (cp == 0) {
      return *this;
    }

    if (getLength() == 0) {
      return String(cp);
    }

    String result;

    auto stringData = std::make_shared<StringData>();
    stringData->resize(getByteLength() + 5);
    std::memcpy(stringData->data(), data(), getByteLength() + 1);

    auto end = utf8catcodepoint(stringData->data() + getByteLength(), cp, 5);
    *end = '\0';

    stringData->resize(end - stringData->data() + 1);

    result.mString = stringData;

    return result;
  }

  String String::operator+(const String &other) const {
    if (other.getLength() == 0) {
      return *this;
    }

    if (getLength() == 0) {
      return other;
    }

    String result;

    auto stringData = std::make_shared<StringData>();
    stringData->resize(getByteLength() + other.getByteLength() + 1);
    std::memcpy(stringData->data(), data(), getByteLength());

    std::memcpy(
      stringData->data() + getByteLength(), other.data(), other.getByteLength()
    );
    stringData->data()[stringData->size() - 1] = '\0';

    result.mString = stringData;

    return result;
  }

  std::size_t String::getLength() const {
    return utf8nlen(data(), getByteLength());
  }

  std::size_t String::getByteLength() const {
    if (std::holds_alternative<const char *>(mString)) {
      return mStringViewByteLength;
    }

    if (std::holds_alternative<std::shared_ptr<StringData>>(mString)) {
      return std::get<std::shared_ptr<StringData>>(mString)->size() - 1;
    }

    return 0;
  }

  bool String::isEmpty() const { return getLength() == 0; }

  bool String::startsWith(const String &other) const {
    if (other.isEmpty()) {
      return true;
    }

    if (getLength() < other.getLength()) {
      return false;
    }

    return utf8ncmp(data(), other.data(), other.getByteLength()) == 0;
  }

  bool String::endsWith(const String &other) const {
    if (other.isEmpty()) {
      return true;
    }

    if (getLength() < other.getLength()) {
      return false;
    }

    return utf8ncmp(
             data() + getByteLength() - other.getByteLength(), other.data(),
             other.getByteLength()
           ) == 0;
  }

  const char *String::c_str() const {
    if (std::holds_alternative<const char *>(mString)) {
      return std::get<const char *>(mString);
    }

    return reinterpret_cast<const char *>(
      std::get<std::shared_ptr<StringData>>(mString)->data()
    );
  }

  const utf8_int8_t *String::data() const {
    return reinterpret_cast<const utf8_int8_t *>(c_str());
  }

  String::CodePoint String::operator[](std::size_t index) const {
    auto ptr = data();

    CodePoint codePoint = 0;

    for (std::size_t offset = 0; offset <= index; ++offset) {
      ptr = utf8codepoint(ptr, &codePoint);
    }

    return codePoint;
  }

  String String::subString(std::size_t start) const {
    if (start >= getLength()) {
      return String();
    }

    const utf8_int8_t *ptr = data();
    CodePoint codePoint = 0;

    auto startPtr = ptr;

    for (std::size_t offset = 0; offset < start; ++offset) {
      startPtr = utf8codepoint(startPtr, &codePoint);
    }

    auto newByteLength = getByteLength() - (startPtr - ptr);

    if (std::holds_alternative<const char *>(mString)) {
      return String(reinterpret_cast<const char *>(startPtr), newByteLength);
    }

    auto stringData = std::make_shared<StringData>();

    String result;
    stringData = std::make_shared<StringData>();
    stringData->resize(newByteLength + 1);
    std::memcpy(stringData->data(), startPtr, newByteLength);
    stringData->data()[newByteLength] = '\0';

    result.mString = stringData;

    return result;
  }

  String String::subString(std::size_t start, std::size_t length) const {
    if (start >= getLength() || length == 0) {
      return String();
    }

    const utf8_int8_t *ptr = data();
    CodePoint codePoint = 0;

    auto startPtr = ptr;

    for (std::size_t offset = 0; offset < start; ++offset) {
      startPtr = utf8codepoint(startPtr, &codePoint);
    }

    auto endPtr = startPtr;

    for (std::size_t offset = 0; offset < length; ++offset) {
      endPtr = utf8codepoint(endPtr, &codePoint);
    }

    auto newByteLength = endPtr - startPtr;

    if (std::holds_alternative<const char *>(mString)) {
      return String(reinterpret_cast<const char *>(startPtr), newByteLength);
    }

    auto stringData = std::make_shared<StringData>();

    String result;
    stringData = std::make_shared<StringData>();
    stringData->resize(newByteLength + 1);
    std::memcpy(stringData->data(), startPtr, newByteLength);
    stringData->data()[newByteLength] = '\0';

    result.mString = stringData;

    return result;
  }

  String::OptionalIndex
  String::indexOf(CodePoint codePoint, std::size_t fromIndex) const {
    if (fromIndex >= getLength()) {
      return OptionalIndex();
    }

    const utf8_int8_t *ptr = data();
    CodePoint currentCodePoint = 0;

    for (std::size_t index = 0; index < fromIndex; ++index) {
      ptr = utf8codepoint(ptr, &currentCodePoint);
    }

    ptr = utf8chr(ptr, codePoint);

    if (ptr == nullptr) {
      return OptionalIndex();
    }

    return OptionalIndex(ptr - data());
  }

  String::OptionalIndex
  String::indexOf(const String &string, std::size_t fromIndex) const {
    if (fromIndex >= getLength()) {
      return OptionalIndex();
    }

    const utf8_int8_t *ptr = data();
    CodePoint currentCodePoint = 0;

    for (std::size_t index = 0; index < fromIndex; ++index) {
      ptr = utf8codepoint(ptr, &currentCodePoint);
    }

    const utf8_int8_t *result = utf8str(ptr, string.data());

    if (result == nullptr) {
      return OptionalIndex();
    }

    return OptionalIndex(result - data());
  }

  String
  String::replace(const String &search, const String &replacement) const {
    if (search.isEmpty() || replacement.isEmpty()) {
      return *this;
    }

    String result;
    std::size_t fromIndex = 0;

    auto optionalIndex = indexOf(search, fromIndex);

    if (optionalIndex.has_value()) {
      auto searchIndex = optionalIndex.value();
      result =
        result + subString(fromIndex, searchIndex - fromIndex) + replacement;
      fromIndex = searchIndex + search.getByteLength();
    }

    return result + subString(fromIndex);
  }

  String
  String::replaceAll(const String &search, const String &replacement) const {
    if (search.isEmpty() || replacement.isEmpty()) {
      return *this;
    }

    String result;
    std::size_t fromIndex = 0;

    while (true) {
      auto optionalIndex = indexOf(search, fromIndex);

      if (!optionalIndex.has_value()) {
        break;
      }

      auto searchIndex = optionalIndex.value();
      result =
        result + subString(fromIndex, searchIndex - fromIndex) + replacement;
      fromIndex = searchIndex + search.getByteLength();
    }

    return result + subString(fromIndex);
  }

  void String::clear() {
    mString = nullString;
    mStringViewByteLength = 0;
  }

  std::list<String> String::split(CodePoint delimiter) const {
    std::list<String> result;
    split(delimiter, std::back_inserter(result));
    return result;
  }

  std::list<String> String::split(const String &delimiter) const {
    std::list<String> result;
    split(delimiter, std::back_inserter(result));
    return result;
  }

  String::Iterator String::begin() const { return Iterator(*this, 0); }

  String::Iterator String::end() const { return Iterator(*this, getLength()); }
} // namespace Luna
