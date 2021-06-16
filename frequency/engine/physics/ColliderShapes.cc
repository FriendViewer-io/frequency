#include "engine/physics/CircleCollider.hh"
#include "engine/physics/ConvexPolyCollider.hh"

#include "engine/core/GObject.hh"

void CircleCollider::commit(Component const& from) {
   ColliderComponent::commit(from);
   CircleCollider const& rhs = static_cast<CircleCollider const&>(from);
   radius = rhs.radius;
}

vec2 CircleCollider::support(vec2 d) const { return parent_data->position + (d.normalized() * radius); }

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
   return max_point;
}