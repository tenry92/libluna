#pragma once

#include <vector>

/**
 * @page 2dspace 2D coordinate system
 *
 * In 2D, the X-axis goes right, the Y-axis goes down.
 * The origin is in the top-left corner of the screen.
 * The unit is pixels.
 *
 * @image html coordinate-system-2d.svg "2D Coordinate System in libluna"
 *
 * In OpenGL, the Y-axis is inverted, i.e. it goes up.
 * The center of the screen is defined as (0, 0) and the screen spans from
 * (-1, -1) (bottom-left) to (1, 1) (top-right).
 * The Z-axis goes out of the screen.
 */
/**
 * @page 3dspace 3D coordinate system
 *
 * The Luna engine uses a right-handed y-up system.
 * The axes (assuming first-person view) are defined as:
 *
 * - X: right
 * - Y: up
 * - Z: backward (out of the screen)
 * 
 * The unit usually is meters.
 *
 * @image html coordinate-system-3d.svg "3D Coordinate System in libluna"
 *
 * In comparison with other 3D engines, there are:
 *
 * - OpenGL, WebGL, Blender, Ogre 3D: right-handed y-up
 * - Direct3D, Metal, Unity: left-handed y-up
 * - Unreal: left-handed z-up
 * - 3DS Max: right-handed z-up
 * - Web (DOM): left-handed y-down
 */
namespace Luna {
  template <typename T> class Vector2 {
    public:
    Vector2();
    Vector2(const Vector2<T>& other);
    template <typename U> Vector2(const Vector2<U>& other);
    Vector2(T x, T y);
    ~Vector2();

    Vector2<T>& operator=(const Vector2<T>& other);
    bool operator==(const Vector2<T>& other) const;
    inline bool operator!=(const Vector2<T>& other) const {
      return !(*this == other);
    }
    Vector2<T> operator+(const Vector2<T>& other) const;
    Vector2<T> operator-(const Vector2<T>& other) const;
    Vector2<T> operator-() const;
    Vector2<T> operator*(T scale) const;
    Vector2<T> operator/(T factor) const;

    T magnitude() const;
    Vector2<T> normalized() const;
    Vector2<T> absolute() const;

    Vector2<T> scaleToFit(const Vector2<T>& other) const;

    static T distance(const Vector2<T>& left, const Vector2<T>& right);

    static constexpr Vector2<T> one() { return Vector2<T>(1, 1); }
    static constexpr Vector2<T> zero() { return Vector2<T>(0, 0); }
    static constexpr Vector2<T> left() { return Vector2<T>(-1, 0); }
    static constexpr Vector2<T> right() { return Vector2<T>(1, 0); }
    static constexpr Vector2<T> up() { return Vector2<T>(0, -1); }
    static constexpr Vector2<T> down() { return Vector2<T>(0, 1); }

    T x{0};
    T y{0};
    T& width = x;
    T& height = y;
  };

  template <typename T> class Vector3 {
    public:
    Vector3();
    Vector3(const Vector3<T>& other);
    Vector3(T x, T y, T z);
    ~Vector3();

    Vector3<T>& operator=(const Vector3<T>& other);
    bool operator==(const Vector3<T>& other) const;
    inline bool operator!=(const Vector3<T>& other) const {
      return !(*this == other);
    }
    Vector3<T> operator+(const Vector3<T>& other) const;
    Vector3<T> operator-(const Vector3<T>& other) const;
    Vector3<T> operator-() const;
    Vector3<T> operator*(T scale) const;
    Vector3<T> operator/(T factor) const;

    T magnitude() const;
    Vector3<T> normalized() const;

    static T distance(const Vector3<T>& left, const Vector3<T>& right);
    static Vector3<T> cross(const Vector3<T>& a, const Vector3<T>& b);

    static constexpr Vector3<T> one() { return Vector3<T>(1, 1, 1); }
    static constexpr Vector3<T> zero() { return Vector3<T>(0, 0, 0); }
    static constexpr Vector3<T> left() { return Vector3<T>(-1, 0, 0); }
    static constexpr Vector3<T> right() { return Vector3<T>(1, 0, 0); }
    static constexpr Vector3<T> up() { return Vector3<T>(0, 1, 0); }
    static constexpr Vector3<T> down() { return Vector3<T>(0, -1, 0); }
    static constexpr Vector3<T> forward() { return Vector3<T>(0, 0, -1); }
    static constexpr Vector3<T> backward() { return Vector3<T>(0, 0, 1); }

    T x{0};
    T y{0};
    T z{0};
  };

  using Vector2i = Vector2<int>;
  using Vector2f = Vector2<float>;
  using Vector3i = Vector3<int>;
  using Vector3f = Vector3<float>;
} // namespace Luna
