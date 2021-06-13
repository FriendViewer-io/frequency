#include "engine/math/AABB.hh"

bool aabb::overlap(aabb const& rhs) const {
   const auto y_test = [](aabb const& lhs, aabb const& rhs) {
      if (lhs.max.y > rhs.min.y && lhs.max.y < rhs.max.y) {
         return true;
      } else if (lhs.min.y > rhs.min.y && lhs.min.y < rhs.max.y) {
         return true;
      } else if (rhs.max.y > lhs.min.y && rhs.max.y < lhs.max.y) {
         return true;
      } else if (rhs.min.y > lhs.min.y && rhs.min.y < lhs.max.y) {
         return true;
      }
      return false;
   };

   if (max.x > rhs.min.x && max.x < rhs.max.x) {
      return y_test(*this, rhs);
   } else if (min.x > rhs.min.x && min.x < rhs.max.x) {
      return y_test(*this, rhs);
   } else if (rhs.max.x > min.x && rhs.max.x < max.x) {
      return y_test(*this, rhs);
   } else if (rhs.min.x > min.x && rhs.min.x < max.x) {
      return y_test(*this, rhs);
   }
   return false;
}

bool aabb::contains(aabb const& rhs) const {
   return (rhs.min.x > min.x && rhs.max.x < max.x &&
           rhs.min.y > min.y && rhs.max.y < max.y);
}