#include "engine/math/Vector.hh"


void vec2::rotate(float angle) {
   *this = rotated(angle);
}

vec2 vec2::rotated(float angle) const {
   vec2 temp;
   temp.x = x * cosf(angle) - y * sinf(angle);
   temp.y = x * sinf(angle) + y * cosf(angle);
   return temp;
}

void vec2::project(vec2 onto) {
   *this = projected(onto);
}

vec2 vec2::projected(vec2 onto) const {
   return onto.normalized() * dot(onto);
}

void vec2::rotate_about(vec2 org, float angle) {
   *this = rotated_about(org, angle);
}

vec2 vec2::rotated_about(vec2 org, float angle) const {
   vec2 tmp = *this - org;
   tmp.rotate(angle);
   return tmp + org;
}

float vec2::angle_between(vec2 other) const {
   return acosf(dot(other) / (mag() * other.mag()));
}