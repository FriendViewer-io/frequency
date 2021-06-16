#pragma once

#include "engine/core/Component.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"

enum class CollisionShape { 
   CIRCLE,
   AABOX_CVP,

   CONVEX_POLY_START = AABOX_CVP,
   CONVEX_POLY_END = AABOX_CVP,
};

class ColliderComponent : public Component {
public:
   ColliderComponent(bool is_static, vec2 start_vel = vec2(), vec2 start_accel = vec2())
       : precomputed_bounds(vec2(), vec2()),
         refresh_bounds(true),
         _is_static(is_static),
         was_static_inserted(false),
         mass(0.f),
         velocity(start_vel),
         acceleration(start_accel) {}

   void on_tick(float dt) override;
   void on_post_tick(float dt) const override {}
   void on_message(GObject* sender, std::string const& msg) override {}

   std::string_view get_component_type_name() const final { return "ColliderComponent"; }
   uint32_t get_component_flags() const override { return EXECUTE_LAST_FLAG; }
   void commit(Component const& from) override;
   aabb const& bounding_box() const { return precomputed_bounds; }
   bool is_static() const { return _is_static; }

   virtual vec2 support(vec2 d) const = 0;
   virtual void update_bounds() = 0;
   virtual CollisionShape get_shape() const = 0;

   virtual ~ColliderComponent();

protected:
   aabb precomputed_bounds;
   bool refresh_bounds;
   bool _is_static;
   bool was_static_inserted;

   float mass;
   vec2 velocity;
   vec2 acceleration;
};