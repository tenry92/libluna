#pragma once

#include <cstring>
#include <functional>
#include <iostream>
#include <list>
#include <string>

#include <libluna/Terminal.hpp>

#ifdef __SWITCH__
#include <switch.h>
#endif

struct Test {
  std::string description;
  std::function<void()> callback;
};

static std::list<Test> tests;
static std::string assertMessage;
static bool testFailed;

static void TEST(const std::string &description, std::function<void()> test) {
  tests.push_back(Test{
      description,
      test,
  });
}

static void ASSERT(bool condition, const std::string &message) {
  if (!testFailed && !condition) {
    assertMessage = message;
    testFailed = true;
  }
}

static void ASSERT_EQL(int actual, int expected, const std::string &message) {
  ASSERT(
      actual == expected, message + ": expected " + std::to_string(actual) +
                              " (actual) to be " + std::to_string(expected) +
                              " (expected)"
  );
}

static void
ASSERT_EQL(float actual, float expected, const std::string &message) {
  ASSERT(
      abs(actual - expected) < 0.0001,
      message + ": expected " + std::to_string(actual) + " (actual) to be " +
          std::to_string(expected) + " (expected)"
  );
}

static void ASSERT_EQL(
    std::string actual, std::string expected, const std::string &message
) {
  ASSERT(
      actual == expected, message + ": expected " + actual +
                              " (actual) to be " + expected + " (expected)"
  );
}

static int runTests() {
  Luna::Terminal::init();

  int index = 0;

  for (auto &test : tests) {
    ++index;
    testFailed = false;
#ifdef N64
    printf("[%d/%d]: %s:", index, tests.size(), test.description.c_str());
#else
    std::cout << "[" << index << "/" << tests.size()
              << "]: " << test.description << ":";
#endif

    try {
      test.callback();
    } catch (std::exception &error) {
      assertMessage = error.what();
      testFailed = true;
    }

    if (testFailed) {
      Luna::Terminal::setColor(Luna::Terminal::brightRed);
#ifdef N64
      printf(" FAILED");
#else
      std::cout << " FAILED";
#endif
      Luna::Terminal::resetColor();
#ifdef N64
      printf("\n  Failed assertion: %s\n", assertMessage.c_str());
#else
      std::cout << "\n  Failed assertion: " << assertMessage << std::endl;
#endif
    } else {
      Luna::Terminal::setColor(Luna::Terminal::brightGreen);
#ifdef N64
      printf(" PASSED");
#else
      std::cout << " PASSED";
#endif
      Luna::Terminal::resetColor();
#ifdef N64
      printf("\n");
#else
      std::cout << std::endl;
#endif
    }
  }

#ifdef __SWITCH__
  while (appletMainLoop()) {
    Luna::Terminal::update();
  }
#endif
#ifdef N64
  while (true) {
    Luna::Terminal::update();
  }
#endif

  Luna::Terminal::quit();

  return 0;
}
