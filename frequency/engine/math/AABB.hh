#pragma once

#include "engine/math/Vector.hh"

struct aabb {
   vec2 min, max;

   aabb(vec2 min = vec2(), vec2 max = vec2()) : min(min), max(max) {}

   void shift(vec2 loc) {
      min += loc;
      max += loc;
   }

   // Check if rhs overlaps this
   bool overlap(aabb const& rhs) const;
   // Check if rhs is inside of this
   bool contains(aabb const& rhs) const;
};