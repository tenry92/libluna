#include <libluna/Test.hpp>
#include <libluna/Vector.hpp>

using namespace Luna;

int main(int, char**) {
  TEST("initial empty Vector2i", []() {
    Vector2i myVector;

    ASSERT_EQL(myVector.x, 0, "x");
    ASSERT_EQL(myVector.y, 0, "y");
  });

  TEST("initial Vector2i", []() {
    Vector2i myVector(1, 2);

    ASSERT_EQL(myVector.x, 1, "x");
    ASSERT_EQL(myVector.y, 2, "y");
  });

  TEST("initial Vector2f", []() {
    Vector2f myVector(3.14, 5.23);

    ASSERT_EQL(myVector.x, 3.14, "x");
    ASSERT_EQL(myVector.y, 5.23, "y");
  });

  TEST("Vector2i + Vector2i", []() {
    Vector2i left(1, 2);
    Vector2i right(3, 4);
    Vector2i sum = left + right;

    ASSERT_EQL(sum.x, 4, "x");
    ASSERT_EQL(sum.y, 6, "x");
  });

  TEST("Vector2i + Vector2i", []() {
    Vector2i left(1, 2);
    Vector2i right(3, 4);
    Vector2i diff = left - right;

    ASSERT_EQL(diff.x, -2, "x");
    ASSERT_EQL(diff.y, -2, "x");
  });

  TEST("equals", []() {
    Vector2i a(1, 2);
    Vector2i b(1, 2);
    Vector2i c(1, 3);

    ASSERT(a == b, "a == b");
    ASSERT(a != c, "a != c");
  });

  TEST("magnitude", []() {
    Vector2i a(4, 0);
    Vector2f b(1, 2);

    ASSERT_EQL(a.magnitude(), 4, "");
    ASSERT_EQL(b.magnitude(), 2.2360680, "");
  });

  TEST("normalize", []() {
    Vector2f a(0.5f, 0.5f);
    Vector2f b(2.0f, 4.0f);

    ASSERT_EQL(a.normalized().magnitude(), 1.0f, "");
    ASSERT_EQL(a.normalized().x, 0.707107f, "");
    ASSERT_EQL(a.normalized().y, 0.707107f, "");

    ASSERT_EQL(b.normalized().magnitude(), 1.0f, "");
    ASSERT_EQL(b.normalized().x, 0.447214f, "");
    ASSERT_EQL(b.normalized().y, 0.894427f, "");
  });

  return runTests();
}
