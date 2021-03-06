#include "engine/core/GObject.hh"
#include "engine/physics/CircleCollider.hh"
#include "engine/physics/ConvexPolyCollider.hh"

void CircleCollider::commit(Component const& from) {
   ColliderComponent::commit(from);
   CircleCollider const& rhs = static_cast<CircleCollider const&>(from);
   radius = rhs.radius;
}

vec2 CircleCollider::support(vec2 d) const { return position() + (d.normalized() * radius); }

void CircleCollider::update_bounds() {
   // set precomputed_bounds min, max
   precomputed_bounds.max = vec2(radius, radius);
   precomputed_bounds.min = vec2(-radius, -radius);
}

vec2 ConvexPolyCollider::support(vec2 d) const {
   d.normalize();
   vec2 max_point;
   float max_distance = -FLT_MAX;
   // d is normalzed
   for (int i = 0; i < get_num_vertices(); i++) {
      vec2 vi = get_vertex(i);
      float distance = vi.dot(d);
      if (distance > max_distance) {
         max_distance = distance;
         max_point = vi;
      }
   }

   return position() + max_point;
}

void ConvexPolyCollider::update_centroid() {
   float a = 0, signed_area = 0;

   for (int i = 0; i < get_num_vertices(); i++) {
      vec2 p0, p1;
      p0 = get_vertex(i);
      p1 = get_vertex((i + 1) % get_num_vertices());
      a = p0.x * p1.y - p1.x * p0.y;
      signed_area += a;
      precomputed_centroid.x += (p0.x + p1.x) * a;
      precomputed_centroid.y += (p0.y + p1.y) * a;
   }

   precomputed_centroid /= (3.f * signed_area);
}