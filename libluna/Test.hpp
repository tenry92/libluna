#pragma once

#include <cstring>
#include <functional>
#include <iostream>
#include <list>
#include <string>

#include <libluna/Console.hpp>

#ifdef __SWITCH__
#include <switch.h>
#endif

#ifdef NDS
#include <nds.h>
#endif

struct Test {
  std::string description;
  std::function<void()> callback;
};

static std::list<Test> tests;
static std::string assertMessage;
static bool testFailed;

static void TEST(const std::string& description, std::function<void()> test) {
  tests.push_back(Test{
    description,
    test,
  });
}

static void ASSERT(bool condition, const std::string& message) {
  if (!testFailed && !condition) {
    assertMessage = message;
    testFailed = true;
  }
}

static void ASSERT_EQL(int actual, int expected, const std::string& message) {
  ASSERT(
    actual == expected, message + ": expected " + std::to_string(actual) +
                          " (actual) to be " + std::to_string(expected) +
                          " (expected)"
  );
}

static void
ASSERT_EQL(float actual, float expected, const std::string& message) {
  ASSERT(
    abs(actual - expected) < 0.0001,
    message + ": expected " + std::to_string(actual) + " (actual) to be " +
      std::to_string(expected) + " (expected)"
  );
}

static void ASSERT_EQL(
  std::string actual, std::string expected, const std::string& message
) {
  ASSERT(
    actual == expected, message + ": expected " + actual + " (actual) to be " +
                          expected + " (expected)"
  );
}

static int runTests() {
  Luna::Console::init();

  int result = 0;

  int index = 0;

  for (auto& test : tests) {
    ++index;
    testFailed = false;
    Luna::Console::write("[{}/{}]: {}:", index, tests.size(), test.description);

    try {
      test.callback();
    } catch (std::exception& error) {
      assertMessage = error.what();
      testFailed = true;
    }

    if (testFailed) {
      result = 1;
      Luna::Console::setColor(Luna::Console::brightRed);
      Luna::Console::write(" FAILED");
      Luna::Console::resetColor();
      Luna::Console::writeLine("\n  Failed assertion: {}", assertMessage);
    } else {
      Luna::Console::setColor(Luna::Console::brightGreen);
      Luna::Console::write(" PASSED");
      Luna::Console::resetColor();
      Luna::Console::writeLine("");
    }
  }

#ifdef __SWITCH__
  while (appletMainLoop()) {
    Luna::Console::update();
  }
#endif
#ifdef NDS
  while (true) {
    swiWaitForVBlank();
    Luna::Console::update();
  }
#endif
#ifdef N64
  while (true) {
    Luna::Console::update();
  }
#endif

  Luna::Console::quit();

  return result;
}
