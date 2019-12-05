#ifndef HYDRA_2D_VEC2
#define HYDRA_2D_VEC2

#include <cmath>
#include <iostream>

namespace hydra_2d {
    class vec2 {
    public:
        double x;
        double y;

        vec2():x{0.0},y{0.0}{}
        vec2(double x,double y):x{x},y{y}{}
        void operator+=(const vec2&);
        void operator-=(const vec2&);
        vec2 operator-();

        vec2& rotate(double);
        vec2& rotate_90();
        vec2& rotate_180();
        vec2& rotate_270();
        vec2& normalize();
        double dist_squared(vec2)const;
        double dot(vec2)const;
        vec2 proj(vec2)const;
        vec2 reflect(vec2)const;
        double mag()const;
    };

    vec2 operator+(const vec2& a, const vec2& b) {
        return vec2{a.x + b.x,a.y + b.y};
    }
    vec2 operator-(const vec2& a, const vec2& b) {
        return vec2{a.x - b.x,a.y - b.y};
    }
    vec2 operator*(const vec2& a, double b) {
        return vec2{a.x * b,a.y * b};
    }
    vec2 operator*(double a, const vec2& b) {
        return vec2{b.x * a, b.y * a};
    }
    std::ostream& operator<<(std::ostream& os,const vec2 v) {
        os<<"("<<v.x<<","<<v.y<<")";
        return os;
    }

    void vec2::operator+=(const vec2& v) {
        x += v.x;
        y += v.y;
    }
    void vec2::operator-=(const vec2& v) {
        x -= v.x;
        y -= v.y;
    }
    vec2 vec2::operator-() {
        return vec2{-x,-y};
    }
    vec2& vec2::rotate(double rot) {
        auto cos_r = std::cos(rot);
        auto sin_r = std::sin(rot);
        *this = vec2 {
            x * cos_r + y * sin_r,
            y * cos_r - x * sin_r
        };
        return *this;
    }
    vec2& vec2::rotate_90() {
        *this = vec2 {y,-x};
        return *this;
    }
    vec2& vec2::rotate_180() {
        *this = vec2 {-x,-y};
        return *this;
    }
    vec2& vec2::rotate_270() {
        *this = vec2 {-y,x};
        return *this;
    }
    vec2& vec2::normalize() {
        double mag = std::sqrt((x * x) + (y * y));
        x /= mag;
        y /= mag;
        return *this;
    }
    double vec2::dist_squared(vec2 v)const {
        double x_diff = x - v.x;
        double y_diff = y - v.y;
        return x_diff * x_diff + y_diff * y_diff;
    }
    double vec2::dot(vec2 v)const {
        return x * v.x + y * v.y;
    }
    vec2 vec2::proj(vec2 v)const {
        vec2 unit_v {v};
        unit_v.normalize();
        double proj_mag = dot(unit_v);
        vec2 proj{*this};
        proj.normalize();
        return proj * proj_mag;
    }
    vec2 vec2::reflect(vec2 normal)const {
        return *this - 2 * (dot(normal)) * normal;
    }
    double vec2::mag()const {
        return std::sqrt(x * x + y * y);
    }
}

#endif