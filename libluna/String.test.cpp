#include <vector>

#include <libluna/String.hpp>
#include <libluna/Test.hpp>

using namespace std;
using namespace Luna;

int main(int, char**) {
  TEST("String emptyString is empty", []() {
    String emptyString;
    ASSERT(strcmp(emptyString.c_str(), "") == 0, "strcmp(..., \"\") == 0");
    ASSERT(emptyString.getLength() == 0, "getLength() == 0");
  });

  TEST("C constructed", []() {
    const char* cString = "c string";
    String cStringConstructed(cString);
    string sString(cString);
    ASSERT(
      strcmp(cStringConstructed.c_str(), cString) == 0,
      "strcmp(..., cString) == 0"
    );
    ASSERT(
      cStringConstructed.getLength() == strlen(cString),
      "getLength() == strlen(cString)"
    );
  });

  TEST("C constructed with NUL", []() {
    const char* cString = "c\0string";
    String cStringConstructed(cString, 8);
    string sString(cString, 8);
    ASSERT(
      strcmp(cStringConstructed.c_str(), cString) == 0,
      "strcmp(..., cString) == 0"
    );
    ASSERT(
      cStringConstructed.getLength() == strlen(cString),
      "getLength() == strlen(cString)"
    );
  });

  TEST("wchar_t constructed", []() {
    const wchar_t* wString = L"wchar_t *ÄÖÜ";
    String wStringConstructed(wString);
    ASSERT(wStringConstructed.getLength() == 12, "getLength() == 12");
  });

  TEST("std:string constructed", []() {
    string sString("std::string");
    String sStringConstructed(sString);
    ASSERT(
      sStringConstructed.getLength() == sString.length(),
      "getLength() == sString.length()"
    );
  });

  TEST("move string (assignment)", []() {
    String originalString("string");
    String str1(originalString);
    String str2;

    ASSERT(str1 == originalString, "str1 is original string");
    ASSERT(str2.isEmpty(), "str2 is empty");

    str2 = std::move(str1);

    ASSERT(str1.isEmpty(), "str1 is empty");
    ASSERT(str2 == originalString, "str2 is original string");
  });

  TEST("move string (constructor)", []() {
    String originalString("string");
    String str1(originalString);
    String str2;

    ASSERT(str1 == originalString, "str1 is original string");
    ASSERT(str2.isEmpty(), "str2 is empty");

    str2 = String(std::move(str1));

    ASSERT(str1.isEmpty(), "str1 is empty");
    ASSERT(str2 == originalString, "str2 is original string");
  });

  TEST("concatenate strings", []() {
    auto catString = String("foo") + String("bar");
    ASSERT(catString == "foobar", "is \"foobar\"");
  });

  TEST("substring", []() {
    String baseString("lorem ipsum dolor");
    ASSERT(
      baseString.subString(6) == "ipsum dolor",
      "subString(6) == \"ipsum dolor\""
    );
    ASSERT(
      baseString.subString(6, 5) == "ipsum", "subString(6, 11) == \"ipsum\""
    );
  });

  TEST("replace", []() {
    String baseString("lorem ipsum dolor");
    ASSERT(
      baseString.replace("ipsum", "replaced") == "lorem replaced dolor",
      "replace 1"
    );
    ASSERT(
      baseString.replaceAll("o", "ö") == "lörem ipsum dölör",
      "replace all occurrences"
    );
  });

  TEST("split into vector", []() {
    std::vector<String> parts;
    String("lorem ipsum dolor").split(" ", std::back_inserter(parts));
    ASSERT(parts.size() == 3, "3 parts");
    ASSERT(parts[0] == "lorem", "part[0] == lorem");
    ASSERT(parts[1] == "ipsum", "part[1] == ipsum");
    ASSERT(parts[2] == "dolor", "part[2] == dolor");
  });

  TEST("for split", []() {
    std::vector<String> parts;

    for (auto&& part : String("lorem ipsum dolor").split(" ")) {
      parts.push_back(part);
    }

    ASSERT(parts.size() == 3, "3 parts");
    ASSERT(parts[0] == "lorem", "part[0] == lorem");
    ASSERT(parts[1] == "ipsum", "part[1] == ipsum");
    ASSERT(parts[2] == "dolor", "part[2] == dolor");
  });

  return runTests();
}
