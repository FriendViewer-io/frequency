#include "engine/physics/CircleCollider.hh"

#include "engine/core/GObject.hh"

void CircleCollider::commit(Component const& from) {
   ColliderComponent::commit(from);
   CircleCollider const& rhs = static_cast<CircleCollider const&>(from);
   radius = rhs.radius;
}

vec2 CircleCollider::support(vec2 d) const { return parent_data->position + (d.normalized() * radius); }