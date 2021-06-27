#pragma once

#include "engine/physics/ColliderComponent.hh"
#include "engine/core/ComponentGen.hh"

class CircleCollider : public ColliderComponent {
   DEFINE_COMPONENT_INLINE_NODEP(CircleCollider, ColliderComponent)

public:
   CircleCollider(bool is_static, float r) : ColliderComponent(is_static), radius(r) {}

   void commit(Component const& from) override;
   std::unique_ptr<Component> clone() {
      return std::make_unique<CircleCollider>(_is_static, radius);
   }

   vec2 support(vec2 d) const override;
   void update_bounds() override;
   void update_centroid() override { precomputed_centroid = vec2(0, 0); }
   CollisionShape get_shape() const override { return CollisionShape::CIRCLE; }

   float get_radius() const { return radius; }

   virtual ~CircleCollider() {}

private:
   float radius;
};