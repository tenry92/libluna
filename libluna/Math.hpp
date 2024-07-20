#pragma once

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Additional math utilities.
 */
namespace Luna::Math {
  constexpr double kPi = M_PI;

  /**
   * @brief Convert degrees (0° - 360°) to radians (0 rad - 2π rad).
   */
  template <typename T> inline constexpr T deg2rad(T degrees) {
    return degrees * static_cast<T>(kPi) / static_cast<T>(180.0);
  }

  /**
   * @brief Convert radians (0 rad - 2π rad) to degress (0° - 360°).
   */
  template <typename T> inline constexpr T rad2deg(T radians) {
    return radians * static_cast<T>(180.0) / static_cast<T>(kPi);
  }

  /**
   * @brief Ping-ping @p value between @p min and @p max.
   */
  template <typename T> inline constexpr T pingPong(T value, T min, T max) {
    T range = max - min;
    value -= min;

    if (range == 0) {
      return min;
    }

    value = std::abs((value % (2 * range)) - range);

    return value + min;
  }

  /**
   * @brief Test if two floating point numbers are approximately equal.
   */
  template <typename T>
  inline constexpr bool
  approxEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon() * 100) {
    return std::fabs(a - b) <= epsilon;
  }

  /**
   * @brief Get closest power of two for @p n.
   *
   * If n is 0, 1 is returned.
   */
  inline constexpr unsigned int closestPowerOfTwo(unsigned int n) {
    if (n == 0) {
      return 1;
    }

    unsigned int nextPower = 1 << (static_cast<unsigned int>(std::log2(n)) + 1);
    unsigned int prevPower = nextPower >> 1;

    return (n - prevPower < nextPower - n) ? prevPower : nextPower;
  }

  /**
   * @brief Check if @p n is a power of two.
   */
  inline constexpr bool isPowerOfTwo(unsigned int n) {
    if (n == 0) {
      return false;
    }

    return (n & (n - 1)) == 0;
  }

  /**
   * @brief Normalize an angle between 0° and 360°.
   */
  template <typename T> inline T normalizeAngle(T angle) {
    angle = std::fmod(angle, 360.0);

    if (angle < 0.0) {
      angle += 360.0;
    }

    return angle;
  }

  /**
   * @brief Linearly interpolate between two values.
   *
   * @param start The start value (if @p t is 0).
   * @param end The end value (if @p t is 1).
   * @param t The interpolation degree.
   */
  template <typename T> inline constexpr T lerp(T start, T end, float t) {
    return (1 - t) * start + +t * end;
  }

  /**
   * @brief Linearly interpolate between two angles.
   *
   * This will take into account that degress like 350° and 10° are close to
   * each other and will interpolate accordingly.
   *
   * @param start The start angle (if @p t is 0).
   * @param end The end angle (if @p t is 1).
   * @param t The interpolation degree.
   */
  template <typename T> inline constexpr T lerpAngle(T start, T end, float t) {
    start = normalizeAngle(start);
    end = normalizeAngle(end);

    T difference = end - start;

    // If the shortest path is the other way around the circle, adjust the difference.
    if (difference > 180.0) {
      difference -= 360.0;
    } else if (difference < -180.0) {
      difference += 360.0;
    }

    T interpolated = start + t * difference;

    return normalizeAngle(interpolated);
  }

  /**
   * @brief Literal symbol for creating radians value from degrees.
   *
   * For example, `360_deg` would yield the value of 2π.
   */
  constexpr double operator"" _deg(long double degrees) {
    return deg2rad(degrees);
  }
} // namespace Luna::Math
