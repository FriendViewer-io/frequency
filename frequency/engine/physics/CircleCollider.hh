#pragma once

#include "engine/physics/ColliderComponent.hh"

class CircleCollider : public ColliderComponent {
public:
   CircleCollider(vec2 sv, vec2 sa, float r) : ColliderComponent(sv, sa), radius(r) {}

   void commit(Component const& from) override;
   std::unique_ptr<Component> clone() { return std::make_unique<CircleCollider>(vec2(), vec2(), radius); }

   vec2 support(vec2 d) const override;
   CollisionShape get_shape() const override { return CollisionShape::CIRCLE; }

   float get_radius() const { return radius; }

   virtual ~CircleCollider() {}

private:
   float radius;
};