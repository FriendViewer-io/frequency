#pragma once

#include <vector>

#include "engine/core/Extension.hh"
#include "engine/math/AABB.hh"

class ColliderComponent;
class SpatialPartition;

class PhysicsExtension : public Extension {
public:
   PhysicsExtension(aabb world_bounds, vec2 cell_dims);

   void extension_init() override {}
   void pre_tick(float dt) override {}
   // Detect [& solve collisions]
   void pre_post_tick(float dt) override;
   void tick_end(float dt) override {}
   void extension_reset() override {}
   vec2 const& get_gravity() const { return gravity; }
   void set_gravity(vec2 const& gravity) { this->gravity = gravity; }

   void object_added(GObject* obj) override;
   void object_removed(GObject* obj) override;

   std::string_view extension_name() const { return "PhysicsExtension"; }

   SpatialPartition& get_dynamic_partition() { return *cell_partition; }
   SpatialPartition& get_static_partition() { return *quadtree; }

private:
   std::vector<ColliderComponent*> colliders;
   SpatialPartition* quadtree;
   SpatialPartition* cell_partition;
   vec2 gravity;
};
