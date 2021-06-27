#pragma once

#include "engine/core/Component.hh"
#include "engine/core/ComponentGen.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"

enum class CollisionShape {
   CIRCLE,
   AABOX_CVP,
   VMESH_CVP,

   CONVEX_POLY_START = AABOX_CVP,
   CONVEX_POLY_END = VMESH_CVP,
};

class ColliderComponent : public Component {
   DEFINE_COMPONENT_INLINE_NODEP(ColliderComponent, Component)

public:
   ColliderComponent(bool is_static)
       : precomputed_bounds(vec2(), vec2()),
         precomputed_centroid(vec2()),
         refresh_bounds(true),
         _is_static(is_static),
         was_static_inserted(false),
         mass(0.f),
         velocity(vec2()),
         acceleration(vec2()),
         gravity_scalar(1.f) {}

   void on_tick(float dt) override;
   void on_post_tick(float dt) const override {}
   void on_message(GObject* sender, std::string const& msg) override {}

   uint32_t get_component_flags() const override { return EXECUTE_LAST_FLAG; }
   void commit(Component const& from) override;
   aabb const& bounding_box() const { return precomputed_bounds; }
   vec2 const& get_centroid() const { return precomputed_centroid; }
   float get_mass() const { return mass; }
   vec2 const& get_velocity() const { return velocity; }
   void set_velocity(vec2 velocity) { this->velocity = velocity; }
   bool is_static() const { return _is_static; }
   void push_object(vec2 direction);
   float get_gravity_scalar() const { return gravity_scalar; }
   void set_gravity_scalar(float gravity) { gravity_scalar = gravity; }

   virtual vec2 support(vec2 d) const = 0;
   virtual void update_bounds() = 0;
   virtual void update_centroid() = 0;
   virtual CollisionShape get_shape() const = 0;

   virtual ~ColliderComponent();

protected:
   aabb precomputed_bounds;
   vec2 precomputed_centroid;
   bool refresh_bounds;
   bool _is_static;
   bool was_static_inserted;

   float mass;
   vec2 velocity;
   vec2 acceleration;
   float gravity_scalar;
};