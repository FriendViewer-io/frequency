#include "engine/physics/CircleCollider.hh"

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