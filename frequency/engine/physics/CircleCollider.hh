#pragma once

#include "engine/physics/ColliderComponent.hh"

class CircleCollider : public ColliderComponent {
public:
   CircleCollider(bool is_static, vec2 sv, vec2 sa, float r) : ColliderComponent(is_static, sv, sa), radius(r) {}

   void commit(Component const& from) override;
   std::unique_ptr<Component> clone() { return std::make_unique<CircleCollider>(_is_static, velocity, acceleration, radius); }

   vec2 support(vec2 d) const override;
   void update_bounds() override;
   CollisionShape get_shape() const override { return CollisionShape::CIRCLE; }

   float get_radius() const { return radius; }

   virtual ~CircleCollider() {}

private:
   float radius;
};