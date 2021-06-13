#include "engine/physics/CollisionHelper.hh"

#include <array>

#include "engine/core/GObject.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/ColliderComponent.hh"

namespace collision {

// Stolen from WinterDev, may be hijacked for EPA
struct Simplex {
   static constexpr size_t MAX_SIMPLEX = 3;
   using iterator = std::array<vec2, 3>::const_iterator;

   std::array<vec2, MAX_SIMPLEX> pts;
   size_t sz;

   Simplex() : pts({vec2(), vec2(), vec2()}), sz(0) {}

   vec2& operator[](uint32_t i) { return pts[i]; }
   size_t size() const { return sz; }
   iterator begin() const { return pts.begin(); }
   iterator end() const { return pts.end(); }

   void add_point(vec2 point) {
      pts = {point, pts[0], pts[1]};
      sz = std::min(sz + 1, MAX_SIMPLEX);
   }

   Simplex& operator=(std::initializer_list<vec2> il) {
      for (auto it = il.begin(); it != il.end(); it++) {
         pts[std::distance(il.begin(), it)] = *it;
      }
      sz = il.size();

      return *this;
   }
};

// Short helpers for repetitive math ops
// support function for minkowski difference
inline vec2 support_mk(ColliderComponent* a, ColliderComponent* b, vec2 v) {
   return a->support(v) - b->support(-v);
}

// computes triple product for 2D: a x b x c
inline vec2 triple_product(vec2 a, vec2 b, vec2 c) { return b * c.dot(a) - a * c.dot(b); }

inline bool same_dir(vec2 a, vec2 b) { return a.dot(b) > 0.f; }

// Simplex handling is a mix of old code & Casey Muratori pseudocode
void best_2simplex(Simplex& shape, vec2& dir_out) {
   const vec2 ab = shape[1] - shape[0], ao = -shape[0];

   // Search perp to AB, origin is in AB region
   if (same_dir(ab, ao)) {
      dir_out = triple_product(ab, ao, ab);
   } else {
      // Search towards AO, closest to A
      shape = {shape[0]};
      dir_out = ao;
   }
}

bool best_3simplex(Simplex& shape, vec2& dir_out) {
   const vec2 ab = shape[1] - shape[0];
   const vec2 ac = shape[2] - shape[0];
   const vec2 ao = -shape[0];
   const vec2 perp_ab = triple_product(ac, ab, ab);
   const vec2 perp_ac = triple_product(ab, ac, ac);

   if (same_dir(perp_ac, ao)) {
      // If true, closest to AC
      if (same_dir(ac, ao)) {
         shape = {shape[0], shape[2]};
         dir_out = triple_product(ac, ao, ac);
         return false;
      } else {
         // Otherwise closest to AB or A
         shape = {shape[0], shape[1]};
         return best_2simplex(shape, dir_out), false;
      }
   } else if (same_dir(perp_ab, ao)) {
      // search on the side of AB, either AB or A
      shape = {shape[0], shape[1]};
      return best_2simplex(shape, dir_out), false;
   } else {
      // Inside of triangle, done
      return true;
   }
   // Shouldn't reach this
   return false;
}

bool next_simplex_and_dir(Simplex& shape, vec2& dir_out) {
   switch (shape.size()) {
      // line simplex never contains origin
      case 2u:
         return best_2simplex(shape, dir_out), false;
      case 3u:
         return best_3simplex(shape, dir_out);
      default:
         return false;
   }
}

// Basic GJK implementation
bool overlap_test_gjk(ColliderComponent* a, ColliderComponent* b) {
   vec2 initial_search =
       a->get_parent()->get_staging_position() - b->get_parent()->get_staging_position();

   if (initial_search == vec2(0.f, 0.f)) {
      initial_search = vec2(1, 0);
   }

   vec2 support_vec = support_mk(a, b, initial_search);

   Simplex simplex;
   simplex.add_point(support_vec);

   vec2 search_dir = -support_vec;

   // Be on the lookout for hanging here
   while (true) {
      support_vec = support_mk(a, b, search_dir);

      if (support_vec.dot(search_dir) <= 0.f) {
         return false;
      }

      simplex.add_point(support_vec);

      // Compute simplex and search direction for next iteration
      // returns true if the input simplex contains the origin
      if (next_simplex_and_dir(simplex, search_dir)) {
         return true;
      }
   }
   // Should never reach here
   return false;
}

}  // namespace collision