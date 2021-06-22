#pragma once

#include <cmath>

struct vec2 {
   union {
      struct {
         float x, y;
      };
      float arr[2];
   };

   vec2(float x = 0, float y = 0) : x(x), y(y) {}
   vec2(vec2 const&) = default;
   vec2& operator=(vec2 const&) = default;

   vec2 operator+(vec2 const& rhs) const { return vec2(x + rhs.x, y + rhs.y); }
   vec2 operator-(vec2 const& rhs) const { return vec2(x - rhs.x, y - rhs.y); }
   vec2 operator*(float rhs) const { return vec2(x * rhs, y * rhs); }
   vec2 operator/(float rhs) const { return vec2(x / rhs, y / rhs); }
   vec2 operator*(vec2 const& rhs) const { return vec2(x * rhs.x, y * rhs.y); }
   vec2 operator/(vec2 const& rhs) const { return vec2(x / rhs.x, y / rhs.y); }

   vec2& operator+=(vec2 const& rhs) {
      x += rhs.x;
      y += rhs.y;
      return *this;
   }
   vec2& operator-=(vec2 const& rhs) {
      x -= rhs.x;
      y -= rhs.y;
      return *this;
   }
   vec2& operator*=(float rhs) {
      x *= rhs;
      y *= rhs;
      return *this;
   }
   vec2& operator/=(float rhs) {
      x /= rhs;
      y /= rhs;
      return *this;
   }
   vec2& operator*=(vec2 const& rhs) {
      x *= rhs.x;
      y *= rhs.y;
      return *this;
   }
   vec2& operator/=(vec2 const& rhs) {
      x /= rhs.x;
      y /= rhs.y;
      return *this;
   }
   float& operator[](size_t idx) { return arr[idx]; }
   float operator[](size_t idx) const { return arr[idx]; }
   vec2 operator-() const { return vec2(-x, -y); }
   bool operator==(vec2 const& rhs) const { return x == rhs.x && y == rhs.y; }

   inline float dot(vec2 const& rhs) const { return x * rhs.x + y * rhs.y; }
   inline float mag_sq() const { return dot(*this); }
   inline float mag() const { return sqrtf(mag_sq()); }
   void normalize() { *this /= mag(); }
   vec2 normalized() const { return *this / mag(); }
   void rotate(float angle);
   vec2 rotated(float angle) const;
   void project(vec2 onto);
   vec2 projected(vec2 onto) const;
   void rotate_about(vec2 org, float angle);
   vec2 rotated_about(vec2 org, float angle) const;

   float angle_between(vec2 other) const;
};

struct mat2 {
   union {
      float mat[2][2];
      float mat_linear[4];
   };

   // fill this in
};