#pragma once

#include "engine/core/Component.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"

enum class CollisionShape { CIRCLE };

class ColliderComponent : public Component {
public:
   ColliderComponent(vec2 start_vel, vec2 start_accel)
       : precomputed_bounds(vec2(), vec2()),
         mass(0.f),
         velocity(start_vel),
         acceleration(start_accel) {}

   void on_tick(float dt) override;
   void on_post_tick(float dt) override {}
   void on_message(GObject* sender, std::string const& msg) override {}

   std::string_view get_component_type_name() const final { return "ColliderComponent"; }
   void commit(Component const& from) override;
   aabb const& bounding_box() const { return precomputed_bounds; }

   virtual vec2 support(vec2 d) const = 0;
   virtual void update_bounds() = 0;
   virtual CollisionShape get_shape() const = 0;

   virtual ~ColliderComponent() {}

protected:
   aabb precomputed_bounds;

private:
   float mass;

   vec2 velocity;
   vec2 acceleration;
};