#pragma once
#include <array>
#include <cmath>

template<size_t DIM, class T> struct vec{
    std::array<T, DIM> data{};
    vec(){};
    T& operator[](const size_t i) { return data[i]; };
    const T& operator[] (const size_t i) const  { return data[i]; }
    void f() {}
};

template <class T> struct vec<2, T> {
    T x, y;
    vec(): x{T()}, y{T()} {}
    vec(T x, T y): x{x}, y {y} {}
    T& operator[](const size_t i) { return i <= 0? x : y; };
    const T& operator[] (const size_t i) const  { return i <= 0? x : y;}  
};

template <class T> struct  vec<3, T> {
    T x, y, z;
    vec(): x{T()}, y{T()}, z{T()} {}
    vec(T x, T y, T z): x{x}, y {y}, z{z} {}
    T& operator[](const size_t i) { return i <= 0 ? x : (i == 1 ? y : z); };
    const T& operator[] (const size_t i) const  {return i <= 0 ? x : (i == 1 ? y : z); }  
    float norm() { return std::sqrt(x*x + y*y + z*z); }
    vec<3, T> normalize() { return *this = (*this) / norm(); } 
};

template<size_t DIM, class T> 
T operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
    T ret = T();
    for (size_t i = 0; i < DIM; i++) { ret += lhs[i] * rhs[i]; }
    return ret;
}

template<size_t DIM, class T> 
vec<DIM, T> operator+(vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
    for (size_t i = 0; i < DIM; i++) { lhs[i] += rhs[i]; }
    return lhs;
}

template<size_t DIM, class T, class U> 
vec<DIM, T> operator+(vec<DIM, T>& lhs, const U& rhs) {
    for (size_t i = 0; i < DIM; i++) { lhs[i] +=  rhs; }
    return lhs;
}

template<size_t DIM, class T> 
vec<DIM, T> operator-(vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
    for (size_t i = 0; i < DIM; i++) { lhs[i] -= rhs[i]; }
    return lhs;
}

template<size_t DIM, class T, class U> 
vec<DIM, T> operator*(vec<DIM, T>& lhs, const U& rhs) {
    for (size_t i = 0; i < DIM; i++) { lhs[i] *=  rhs; }
    return lhs;
}

template<size_t DIM, class T, class U> 
vec<DIM, T> operator/(vec<DIM, T>& lhs, const U& rhs) {
    for (size_t i = 0; i < DIM; i++) { lhs[i] /= rhs; }
    return lhs;
}

template <class T> vec<3, T> cross(vec<3,T> v1, vec<3,T> v2) {
    return vec<3,T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

template<size_t DIM_ROW, size_t DIM_COL, class T> struct mat {
    std::array<std::array<T, DIM_COL>, DIM_ROW> data;
    mat() {}
    std::array<T, DIM_COL>& operator[](const size_t idx) { return data[idx]; }
    const std::array<T, DIM_COL>& operator[](const size_t idx) const { return data[idx]; }
};

typedef vec<2, float> vec2f;
typedef vec<2, int> vec2i;
typedef vec<3, float> vec3f;
typedef vec<3, int> vec3i;
typedef vec<4, float> vec4f;
typedef mat<4,4, float> mat4i;