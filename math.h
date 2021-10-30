#pragma once
#include <array>
#include <cmath>
#include <iostream>
template<size_t DIM, class T> struct vec {
    std::array<T, DIM> data = {};
    T& operator[](const size_t i) { return data[i]; }
    const T& operator[](const size_t i) const { return data[i]; }
    vec<DIM, T>& normalize() {
        double norm = 0;
        for (const auto& elem: data) {
            norm += elem * elem;
        }
        norm = std::sqrt(norm);
        if (norm < 1e-10) {
            return *this;
        }
        for(auto& elem: data) {
            elem /= norm;
        }
        return *this;
    }
    vec<DIM+1, T> increment_dim(T last = 0) {
        vec<DIM+1, T> rv {};
        for (int i = 0; i < DIM; i++) {
            rv.data[i] = this -> data[i];
        }
        rv[DIM] = last;
        return rv;
    }
    vec<DIM-1, T> reduce_dim() {
        vec<DIM-1, T> rv{};
        for (int i = 0; i < DIM - 1; i++) {
            rv.data[i] = this -> data[i];
        }
        return rv;
    }
    template<class U>
    vec<DIM, U> astype() {
        vec<DIM, U> rv{};
        for (int i = 0; i < DIM; i++) {
            rv.data[i] = this -> data[i];
        }
        return rv;
    }
    vec();
    vec(T x) ;
    vec(T x, T y);
    vec(T x, T y, T z);
    vec(T x, T y, T z, T w);
    vec(std::array<T, DIM> data): data{data}{}
};

template<size_t DIM, class T> vec<DIM, T>::vec(): data{} {}
template<size_t DIM, class T> vec<DIM, T>::vec(T x): data{} {
    if (DIM >= 1) { data[0] = x; }
}
template<size_t DIM, class T> vec<DIM, T>::vec(T x, T y): data{} {
    if (DIM >= 1) { data[0] = x; }
    if (DIM >= 2) { data[1] = y; }
}

template<size_t DIM, class T> vec<DIM, T>::vec(T x, T y, T z): data{} {
    if (DIM >= 1) { data[0] = x; }
    if (DIM >= 2) { data[1] = y; }
    if (DIM >= 3) { data[2] = z; }
}

template<size_t DIM, class T> vec<DIM, T>::vec(T x, T y, T z, T w): data{} {
    if (DIM >= 1) { data[0] = x; }
    if (DIM >= 2) { data[1] = y; }
    if (DIM >= 3) { data[2] = z; }
    if (DIM >= 4) { data[3] = w; }
}

template<size_t DIM, class T> 
vec<DIM, T> operator+(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
    vec<DIM, T> rv;
    for (size_t i = 0; i < DIM; i++) { rv[i] = lhs[i] + rhs[i]; }
    return rv;
}

template<size_t DIM, class T, class U> 
vec<DIM, T> operator+(const vec<DIM, T>& lhs, const U& rhs) {
    vec<DIM, T> rv;
    for (size_t i = 0; i < DIM; i++) { rv[i] = lhs[i] + rhs; }
    return rv;
}

template<size_t DIM, class T> 
vec<DIM, T> operator-(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
    vec<DIM, T> rv;
    for (size_t i = 0; i < DIM; i++) { rv[i] = lhs[i] - rhs[i]; }
    return rv;
}

template<size_t DIM, class T> 
vec<DIM, T> operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
    vec<DIM, T> rv;
    for (size_t i = 0; i < DIM; i++) { rv[i] = lhs[i] * rhs[i]; }
    return rv;
}

template<size_t DIM, class T, class U> 
vec<DIM, T> operator*(const vec<DIM, T>& lhs, const U& rhs) {
    vec<DIM, T> rv;
    for (size_t i = 0; i < DIM; i++) { rv[i] = lhs[i] * rhs; }
    return rv;
}

template<size_t DIM, class T, class U> 
vec<DIM, T> operator/(const vec<DIM, T>& lhs, const U& rhs) {
    vec<DIM, T> rv;
    for (size_t i = 0; i < DIM; i++) { rv[i] = lhs[i] / rhs; }
    return rv;
}
template<size_t DIM, class T> 
T dot(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
    auto temp{lhs * rhs};
    T rv {};
    for (int i = 0; i < DIM; i++) { rv += temp[i]; }
    return rv;
}

template<size_t DIM, class T> 
vec<3, T> cross(const vec<3, T>& v1, const vec<3, T>& v2) {
    return {v1[1]*v2[2] - v1[2]*v2[1], v1[2]*v2[0] - v1[0]*v2[2], v1[0]*v2[1] - v1[1]*v2[0]};
}

typedef vec<2,  float> vec2f;
typedef vec<2,  int>   vec2i;
typedef vec<3,  float> vec3f;
typedef vec<3,  int>   vec3i;
typedef vec<4,  float> vec4f;

template<size_t DIM, class T> struct mat {
    std::array<std::array<T, DIM>, DIM> data;
    std::array<T, DIM>& operator[](const size_t i) { return data[i]; }
    const std::array<T, DIM>& operator[](const size_t i) const { return data[i]; }
    vec<DIM, T> get_row(size_t idx) const;
    vec<DIM, T> get_col(size_t idx) const;
    void set_row(size_t idx, vec<DIM, T>);
    void set_col(size_t idx, vec<DIM, T>);
    mat();
};

template<size_t DIM, class T> 
void mat<DIM, T>::set_row(size_t idx, vec<DIM, T> v)  {
    data[idx] = v;
}

template<size_t DIM, class T> 
void mat<DIM, T>::set_col(size_t idx, vec<DIM, T> v)  {
    for (int i = 0; i < DIM; i++) {
        data[i][idx] = v[i];
    }
}

template<size_t DIM, class T> 
vec<DIM, T> mat<DIM, T>::get_row(size_t idx) const {
    return vec<DIM,T>{data[idx]};
}

template<size_t DIM, class T> 
vec<DIM, T> mat<DIM, T>::get_col(size_t idx) const {
    vec<DIM, T> rv{};
    for (int i = 0; i < DIM; i++) {
        rv[i] = data[i][idx];
    }
    return rv;
}

template<size_t DIM, class T> mat<DIM, T>::mat() {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            data[i][j] = 0;
        }
    }
}

template<size_t DIM, class T> 
mat<DIM, T> identity(void) {
    mat<DIM, T> rv{};
    for (int i = 0; i < DIM; i++) {
        rv[i][i] = 1;
    }
    return rv;
}

template<size_t DIM, class T>
mat<DIM, T> operator*(mat<DIM, T> lhs, mat<DIM, T> rhs) {
    mat<DIM, T> rv {};
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            rv.data[i][j] = dot(lhs.get_row(i), rhs.get_col(j));
        }
    }
    return rv;
}

template<size_t DIM, class T>
vec<DIM, T> operator*(mat<DIM, T> lhs, vec<DIM, T> rhs) {
    vec<DIM, T> rv {};
    for (int i = 0; i < DIM; i++) {
        rv.data[i] = dot(lhs.get_row(i), rhs);
    }
    return rv;
}

template<size_t DIM, class T>
std::ostream& operator<<(std::ostream& os, const vec<DIM, T>& input) {
    for (auto x: input.data) {
        os << x << " ";
    }
    return os;
}

template<size_t DIM, class T>
std::ostream& operator<<(std::ostream& os, const mat<DIM, T>& input) {
    for (auto x: input.data) {
        os << vec<DIM,T>{x} << std::endl;
    }
    return os;
}

typedef mat<4, float> mat4f;
typedef mat<3, float> mat3f;


