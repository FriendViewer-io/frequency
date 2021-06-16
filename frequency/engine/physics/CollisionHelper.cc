#include "engine/physics/CollisionHelper.hh"

#include <array>

#include "engine/core/GObject.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/CircleCollider.hh"
#include "engine/physics/ColliderComponent.hh"
#include "engine/physics/ConvexPolyCollider.hh"

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

bool overlap_test_circle_circle(CircleCollider* a, CircleCollider* b) {
   vec2 a_center = a->get_parent()->get_position();
   vec2 b_center = b->get_parent()->get_position();
   const float double_rad = a->get_radius() + b->get_radius();
   vec2 ab = b_center - a_center;
   if (ab.mag_sq() < (double_rad * double_rad)) {
      return true;
   }
   return false;
}

bool overlap_test_circle_convex(CircleCollider* a, ConvexPolyCollider* b) {
   enum Feature { EDGE, VERTEX };
   float min_dist_squared = FLT_MAX;
   Feature nearest_feature;
   vec2 nearest_point;
   vec2 nearest_edge;

   const vec2 a_pos = a->get_parent()->get_position();
   const float radius = a->get_radius();

   for (int i = 0; i < b->get_num_vertices(); i++) {
      vec2 vi = b->get_vertex(i);
      vec2 vj = b->get_vertex((i + 1) & b->get_num_vertices());

      vec2 ij = vj - vi;
      vec2 ia = a_pos - vi;

      vec2 edge_dir = ij.normalized();
      // named G
      vec2 point_on_ij = edge_dir * edge_dir.dot(ia) + vi;

      vec2 ig = point_on_ij - vi;
      vec2 jg = point_on_ij - vj;

      if (ig.dot(jg) < 0.f) {
         const float distance_to_circle = (a_pos - point_on_ij).mag_sq();
         if (distance_to_circle < min_dist_squared) {
            min_dist_squared = distance_to_circle;
            nearest_feature = EDGE;
            nearest_edge = ij;
            nearest_point = point_on_ij;
         }
      }  // otherwise ignore this point

      const float distance_to_vi = (vi - a_pos).mag_sq();
      if (distance_to_vi < min_dist_squared) {
         min_dist_squared = distance_to_vi;
         nearest_feature = VERTEX;
         nearest_point = vi;
      }
   }

   return (nearest_point - a_pos).mag_sq() < (radius * radius);
}

bool overlap_test(ColliderComponent* a, ColliderComponent* b) {
   CollisionShape a_shape = a->get_shape();
   CollisionShape b_shape = b->get_shape();

   if (a_shape == CollisionShape::CIRCLE && b_shape == CollisionShape::CIRCLE) {
      return overlap_test_circle_circle(static_cast<CircleCollider*>(a),
                                        static_cast<CircleCollider*>(b));

   } else if (a_shape == CollisionShape::CIRCLE && b_shape >= CollisionShape::CONVEX_POLY_START &&
              a_shape <= CollisionShape::CONVEX_POLY_END) {
      return overlap_test_circle_convex(static_cast<CircleCollider*>(a),
                                        static_cast<ConvexPolyCollider*>(b));

   } else if (b_shape == CollisionShape::CIRCLE && a_shape >= CollisionShape::CONVEX_POLY_START &&
              a_shape <= CollisionShape::CONVEX_POLY_END) {
      return overlap_test_circle_convex(static_cast<CircleCollider*>(b),
                                        static_cast<ConvexPolyCollider*>(a));

   } else {
      return overlap_test_gjk(a, b);
   }
}

void generate_contacts_circle_circle(CircleCollider* a, CircleCollider* b,
                                     ContactManifold& manifold) {
   vec2 ca = a->get_parent()->get_position();
   vec2 cb = b->get_parent()->get_position();
   vec2 resolve_dir = cb - ca;

   const float dist = resolve_dir.mag();
   resolve_dir /= dist;

   manifold.a = a;
   manifold.b = b;
   manifold.normal = resolve_dir;
   manifold.tangent = vec2(-resolve_dir.y, resolve_dir.x);
   manifold.pa = ca + resolve_dir * a->get_radius();
   manifold.pb = cb - resolve_dir * b->get_radius();
   manifold.overlap_distance = (a->get_radius() + b->get_radius()) - dist;
}

void generate_contacts(ColliderComponent* a, ColliderComponent* b, ContactManifold& manifold) {
   CollisionShape a_shape = a->get_shape();
   CollisionShape b_shape = b->get_shape();

   if (a_shape == CollisionShape::CIRCLE && b_shape == CollisionShape::CIRCLE) {
      return generate_contacts_circle_circle(static_cast<CircleCollider*>(a),
                                             static_cast<CircleCollider*>(b), manifold);

   } else if (a_shape == CollisionShape::CIRCLE && b_shape >= CollisionShape::CONVEX_POLY_START &&
              a_shape <= CollisionShape::CONVEX_POLY_END) {
      // return overlap_test_circle_convex(static_cast<CircleCollider*>(a),
      //                                   static_cast<ConvexPolyCollider*>(b));

   } else if (b_shape == CollisionShape::CIRCLE && a_shape >= CollisionShape::CONVEX_POLY_START &&
              a_shape <= CollisionShape::CONVEX_POLY_END) {
      // return overlap_test_circle_convex(static_cast<CircleCollider*>(b),
      //                                   static_cast<ConvexPolyCollider*>(a));

   } else {
      // return overlap_test_gjk(a, b);
   }
}

}  // namespace collision