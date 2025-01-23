#include <libluna/Rect.hpp>

using namespace Luna;

template <typename T> Rect<T>::Rect() = default;

template <typename T> Rect<T>::Rect(const Rect<T>& other) {
  this->x = other.x;
  this->y = other.y;
  this->width = other.width;
  this->height = other.height;
}

template <typename T> Rect<T>::Rect(T pX, T pY, T pWidth, T pHeight) {
  this->x = pX;
  this->y = pY;
  this->width = pWidth;
  this->height = pHeight;
}

template <typename T> Rect<T>::~Rect() = default;

template <typename T> Rect<T>& Rect<T>::operator=(const Rect<T>& other) {
  this->x = other.x;
  this->y = other.y;
  this->width = other.width;
  this->height = other.height;

  return *this;
}

template <typename T> bool Rect<T>::operator==(const Rect<T>& other) const {
  return this->x == other.x && this->y == other.y &&
         this->width == other.width && this->height == other.height;
}

template class Luna::Rect<int>;
template class Luna::Rect<float>;
