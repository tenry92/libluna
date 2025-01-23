#include <algorithm>
#include <cmath>

#include <libluna/Vector.hpp>

using namespace Luna;

template <typename T> Vector2<T>::Vector2() = default;

template <typename T> Vector2<T>::Vector2(const Vector2<T> &other) {
  this->x = other.x;
  this->y = other.y;
}

template <typename T>
template <typename U>
Vector2<T>::Vector2(const Vector2<U> &other) {
  this->x = static_cast<T>(other.x);
  this->y = static_cast<T>(other.y);
}

template <typename T> Vector2<T>::Vector2(T pX, T pY) {
  this->x = pX;
  this->y = pY;
}

template <typename T> Vector2<T>::~Vector2() = default;

template <typename T>
Vector2<T> &Vector2<T>::operator=(const Vector2<T> &other) {
  this->x = other.x;
  this->y = other.y;

  return *this;
}

template <typename T>
bool Vector2<T>::operator==(const Vector2<T> &other) const {
  return this->x == other.x && this->y == other.y;
}

template <typename T>
Vector2<T> Vector2<T>::operator+(const Vector2<T> &other) const {
  return Vector2<T>(this->x + other.x, this->y + other.y);
}

template <typename T>
Vector2<T> Vector2<T>::operator-(const Vector2<T> &other) const {
  return Vector2<T>(this->x - other.x, this->y - other.y);
}

template <typename T> Vector2<T> Vector2<T>::operator-() const {
  return Vector2<T>(-this->x, -this->y);
}

template <typename T> Vector2<T> Vector2<T>::operator*(T scale) const {
  return Vector2<T>(this->x * scale, this->y * scale);
}

template <typename T> Vector2<T> Vector2<T>::operator/(T scale) const {
  return Vector2<T>(this->x / scale, this->y / scale);
}

template <typename T> T Vector2<T>::magnitude() const {
  return static_cast<T>(std::sqrt(std::pow(this->x, 2) + std::pow(this->y, 2)));
}

template <typename T> Vector2<T> Vector2<T>::normalized() const {
  auto length = this->magnitude();

  return Vector2<T>(x / length, y / length);
}

template <typename T> Vector2<T> Vector2<T>::absolute() const {
  return Vector2<T>(std::abs(x), std::abs(y));
}

template <typename T>
Vector2<T> Vector2<T>::scaleToFit(const Vector2<T> &other) const {
  float scaleWidth = static_cast<float>(other.x) / static_cast<float>(this->x);
  float scaleHeight = static_cast<float>(other.y) / static_cast<float>(this->y);

  float scaleFactor = std::min(scaleWidth, scaleHeight);

  return Vector2<T>(
    static_cast<T>(static_cast<float>(this->x) * scaleFactor),
    static_cast<T>(static_cast<float>(this->y) * scaleFactor)
  );
}

template <typename T>
T Vector2<T>::distance(const Vector2<T> &left, const Vector2<T> &right) {
  return (left - right).magnitude();
}

template class Luna::Vector2<int>;
template class Luna::Vector2<float>;
template Luna::Vector2<float>::Vector2(const Vector2<int> &);

template <typename T> Vector3<T>::Vector3() = default;

template <typename T> Vector3<T>::Vector3(const Vector3<T> &other) {
  this->x = other.x;
  this->y = other.y;
  this->z = other.z;
}

template <typename T> Vector3<T>::Vector3(T pX, T pY, T pZ) {
  this->x = pX;
  this->y = pY;
  this->z = pZ;
}

template <typename T> Vector3<T>::~Vector3() = default;

template <typename T>
Vector3<T> &Vector3<T>::operator=(const Vector3<T> &other) {
  this->x = other.x;
  this->y = other.y;
  this->z = other.z;

  return *this;
}

template <typename T>
bool Vector3<T>::operator==(const Vector3<T> &other) const {
  return this->x == other.x && this->y == other.y && this->z == other.z;
}

template <typename T>
Vector3<T> Vector3<T>::operator+(const Vector3<T> &other) const {
  return Vector3<T>(this->x + other.x, this->y + other.y, this->z + other.z);
}

template <typename T>
Vector3<T> Vector3<T>::operator-(const Vector3<T> &other) const {
  return Vector3<T>(this->x - other.x, this->y - other.y, this->z - other.z);
}

template <typename T> Vector3<T> Vector3<T>::operator-() const {
  return Vector3<T>(-this->x, -this->y, -this->z);
}

template <typename T> Vector3<T> Vector3<T>::operator*(T scale) const {
  return Vector3<T>(this->x * scale, this->y * scale, this->z * scale);
}

template <typename T> Vector3<T> Vector3<T>::operator/(T scale) const {
  return Vector3<T>(this->x / scale, this->y / scale, this->z / scale);
}

template <typename T> T Vector3<T>::magnitude() const {
  return static_cast<T>(std::sqrt(
    std::pow(this->x, 2) + std::pow(this->y, 2) + std::pow(this->z, 2)
  ));
}

template <typename T> Vector3<T> Vector3<T>::normalized() const {
  auto length = this->magnitude();

  return Vector3<T>(x / length, y / length, z / length);
}

template <typename T>
T Vector3<T>::distance(const Vector3<T> &left, const Vector3<T> &right) {
  return (left - right).magnitude();
}

template <typename T>
Vector3<T> Vector3<T>::cross(const Vector3<T> &a, const Vector3<T> &b) {
  return Vector3<T>(
    a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x
  );
}

template class Luna::Vector3<int>;
template class Luna::Vector3<float>;
