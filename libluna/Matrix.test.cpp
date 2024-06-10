#include <iomanip>
#include <iostream>

#include <libluna/Matrix.hpp>
#include <libluna/Terminal.hpp>
#include <libluna/Test.hpp>

using namespace Luna;

static void printMatrix(const Matrix4x4 &matrix) {
  auto valuePtr = matrix.getValuePointer();

  std::cout << "\n----------" << std::endl;

  for (int row = 0; row < 4; ++row) {
    std::cout << "[";
    for (int col = 0; col < 4; ++col) {
      std::cout << std::setw(2) << std::setprecision(2)
                << valuePtr[row + col * 4];
      if (col < 3) {
        std::cout << ", ";
      }
    }
    std::cout << "]" << std::endl;
  }

  std::cout << "----------" << std::endl;
}

static void compareMatrix(const Matrix4x4 &actual, const Matrix4x4 &expected) {
  auto actualPtr = actual.getValuePointer();
  auto expectedPtr = expected.getValuePointer();

  std::cout << "\n----------" << std::endl;

  std::cout << "  actual             expected" << std::endl;

  for (int row = 0; row < 4; ++row) {
    std::cout << "[";
    for (int col = 0; col < 4; ++col) {
      auto thisValue = actualPtr[row + col * 4];
      auto otherValue = expectedPtr[row + col * 4];

      if (abs(thisValue - otherValue) >= 0.05f) {
        Terminal::setColor(Terminal::red);
      }

      std::cout << std::setw(2) << std::setprecision(2) << thisValue;

      Terminal::resetColor();

      if (col < 3) {
        std::cout << ", ";
      }
    }
    std::cout << "]   ";

    std::cout << "[";
    for (int col = 0; col < 4; ++col) {
      auto thisValue = expectedPtr[row + col * 4];
      auto otherValue = actualPtr[row + col * 4];

      if (abs(thisValue - otherValue) >= 0.05f) {
        Terminal::setColor(Terminal::green);
      }

      std::cout << std::setw(2) << std::setprecision(2) << thisValue;

      Terminal::resetColor();

      if (col < 3) {
        std::cout << ", ";
      }
    }
    std::cout << "]" << std::endl;
  }

  std::cout << "----------" << std::endl;
}

static void ASSERT_EQL(const Matrix4x4 &actual, const Matrix4x4 &expected) {
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      auto delta = abs(actual.at(row, col) - expected.at(row, col));

      ASSERT(delta < 0.05f, "");

      if (delta >= 0.05f) {
        compareMatrix(actual, expected);
        return;
      }
    }
  }
}

int main(int, char **) {
  TEST("identify matrix", []() {
    auto actual = Matrix4x4::identity();
    auto expected = Matrix4x4({{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }});

    ASSERT_EQL(actual, expected);
  });

  TEST("translate", []() {
    auto actual = Matrix4x4::identity().translate({10, 20, 30});
    auto expected = Matrix4x4({{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {10, 20, 30, 1},
    }});

    ASSERT_EQL(actual, expected);
  });

  TEST("rotateX", []() {
    auto actual = Matrix4x4::identity().rotateX(42);
    auto expected = Matrix4x4({{
        {1, 0, 0, 0},
        {0, -0.4, -0.916, 0},
        {0, 0.916, -0.4, 0},
        {0, 0, 0, 1},
    }});

    ASSERT_EQL(actual, expected);
  });

  TEST("rotateY", []() {
    auto actual = Matrix4x4::identity().rotateY(42);
    auto expected = Matrix4x4({{
        {-0.4, 0, 0.92, 0},
        {0, 1, 0, 0},
        {0, 0, -0.4, 0},
        {-0.92, 0, -0.4, 1},
    }});

    ASSERT_EQL(actual, expected);
  });

  TEST("rotateZ", []() {
    auto actual = Matrix4x4::identity().rotateZ(42);
    auto expected = Matrix4x4({{
        {-0.4, -0.92, 0, 0},
        {0.92, -0.4, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }});

    ASSERT_EQL(actual, expected);
  });

  TEST("perspective", []() {
    auto actual =
        Matrix4x4::identity().perspective(45, 16.0f / 9.0f, 0.1f, 100.0f);
    auto expected = Matrix4x4({{
        {1.4, 0, 0, 0},
        {0, 2.4, 0, 0},
        {0, 0, -1, -1},
        {0, 0, -0.2, 0},
    }});

    ASSERT_EQL(actual, expected);
  });

  TEST("view * transform", []() {
    auto transform = Matrix4x4::identity();
    auto view =
        Matrix4x4::perspective(45.0f, 1920.0f / 1080.0f, 0.01f, 2000.0f);
    view = view.translate({0.0f, -1.0f, 0.0f});
    auto actual = view * transform;

    auto expected = Matrix4x4({{
        {1.4, 0, 0, 0},
        {0, 2.4, 0, 0},
        {0, 0, -1, -1},
        {0, -2.4, -0.02, 0},
    }});

    ASSERT_EQL(actual, expected);
  });

  TEST("project * view * model", []() {
    auto model = Matrix4x4::identity();
    auto projection =
        Matrix4x4::perspective(45.0f, 1920.0f / 1080.0f, 0.01f, 2000.0f);
    auto view = Matrix4x4::identity().translate({0.0f, 0.0f, -3.0f});

    auto actual = projection * view * model;
    auto expected = Matrix4x4({{
        {1.4, 0, 0, 0},
        {0, 2.4, 0, 0},
        {0, 0, -1, -1},
        {0, 0, 3, 3},
    }});

    ASSERT_EQL(actual, expected);
  });

  return runTests();
}
