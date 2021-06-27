#pragma once

#include "engine/core/ComponentGen.hh"
#include "engine/physics/ConvexPolyCollider.hh"

class AABoxCollider : public ConvexPolyCollider {
   DEFINE_COMPONENT_INLINE_NODEP(AABoxCollider, ConvexPolyCollider)
public:
   AABoxCollider(vec2 half_dims, bool s) : ConvexPolyCollider(s), half_dims(half_dims) {}

   int get_num_vertices() const override { return 4; }

   vec2 get_vertex(int index) const override {
      switch (index) {
         case 0:
            return vec2(-half_dims.x, half_dims.y);
         case 1:
            return vec2(half_dims.x, half_dims.y);
         case 2:
            return vec2(half_dims.x, -half_dims.y);
         case 3:
            return vec2(-half_dims.x, -half_dims.y);
         default:
            return vec2();
      }
   }

   void commit(Component const& from) override {
      ConvexPolyCollider::commit(from);

      AABoxCollider const& rhs = static_cast<AABoxCollider const&>(from);
      half_dims = rhs.half_dims;
   }

   std::unique_ptr<Component> clone() {
      return std::make_unique<AABoxCollider>(half_dims, is_static());
   }

   void update_bounds() override { precomputed_bounds = aabb(-half_dims, half_dims); }

   CollisionShape get_shape() const { return CollisionShape::AABOX_CVP; }

private:
   vec2 half_dims;
};