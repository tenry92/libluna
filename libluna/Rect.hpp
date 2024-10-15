#pragma once

namespace Luna {
  template <typename T> class Rect {
    public:
    Rect();
    Rect(const Rect<T> &other);
    Rect(T x, T y, T width, T height);
    ~Rect();

    Rect<T> &operator=(const Rect<T> &other);
    bool operator==(const Rect<T> &other) const;
    inline bool operator!=(const Rect<T> &other) const {
      return !(*this == other);
    }

    inline T area() const { return width * height; }

    T x{0};
    T y{0};
    T width{0};
    T height{0};
  };

  using Recti = Rect<int>;
  using Rectf = Rect<float>;
} // namespace Luna
