#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>
#include <vector>

class Matrix;

template <class T>
class Vec2 {
   public:
    T x, y;
    Vec2<T>() : x(T()), y(T()) {}
    Vec2<T>(T _x, T _y) : x(_x), y(_y) {}

    Vec2<T> operator+(const Vec2<T>& V) const {
        return Vec2<T>(x + V.x, y + V.y);
    }

    Vec2<T> operator-(const Vec2<T>& V) const {
        return Vec2<T>(x - V.x, y - V.y);
    }

    Vec2<T> operator*(float f) const { return Vec2<T>(x * f, y * f); }

    T& operator[](const int i) { return i <= 0 ? x : y; }

    template <class>
    friend std::ostream& operator<<(std::ostream& s, Vec2<T>& v);
};

template <class T>
class Vec3 {
   public:
    T x, y, z;
    Vec3<T>() : x(T()), y(T()), z(T()) {}
    Vec3<T>(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

    Vec3<T>(Matrix m);

    template <class U>
    Vec3<T>(const Vec3<U>& v);

    Vec3<T> operator^(const Vec3<T>& v) const {
        return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    Vec3<T> operator+(const Vec3<T>& v) const {
        return Vec3<T>(x + v.x, y + v.y, z + v.z);
    }

    Vec3<T> operator-(const Vec3<T>& v) const {
        return Vec3<T>(x - v.x, y - v.y, z - v.z);
    }

    Vec3<T> operator*(float f) const { return Vec3<T>(x * f, y * f, z * f); }

    T operator*(const Vec3<T>& v) const { return x * v.x + y * v.y + z * v.z; }

    float norm() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3<T>& normalize(T l = 1) {
        *this = (*this) * (l / norm());
        return *this;
    }

    T& operator[](const int i) { return i <= 0 ? x : (1 == i ? y : z); }

    template <class>
    friend std::ostream& operator<<(std::ostream& s, Vec3<T>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

template <>
template <>
Vec3<int>::Vec3(const Vec3<float>& v);

template <>
template <>
Vec3<float>::Vec3(const Vec3<int>& v);

template <class T>
std::ostream& operator<<(std::ostream& s, Vec2<T>& v) {
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <class T>
std::ostream& operator<<(std::ostream& s, Vec3<T>& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

//////////////////////////////////////////////////////////////////////////////////////////////

class Matrix {
    std::vector<std::vector<float> > m;
    int rows, cols;

   public:
    Matrix(int r = 4, int c = 4);
    Matrix(Vec3f v);
    int nrows();
    int ncols();
    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int i);
    Matrix operator*(const Matrix& a) const;
    Vec2f operator*(Vec3f v) const;
    Matrix transpose();
    Matrix inverse();
    void set_col(int col, Vec2f v);
    friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

#endif  //__GEOMETRY_H__