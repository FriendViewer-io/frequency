#include "engine/physics/ColliderComponent.hh"

#include "engine/core/GObject.hh"
#include "engine/core/StateManager.hh"
#include "engine/physics/PhysicsExtension.hh"
#include "engine/physics/SpatialPartition.hh"

void ColliderComponent::on_tick(float dt) {
   if (refresh_bounds) {
      refresh_bounds = false;
      update_bounds();
   }

   
   if (!_is_static) {
      // remove and reinsert
      statemgr::get_physics_extension()->get_dynamic_partition().remove(this);
      // insert & remove from cell-based partition
      parent_data->position = (acceleration * dt * dt) * 0.5f + (velocity * dt) + parent_data->position;
      velocity = (acceleration * dt) + velocity;
      statemgr::get_physics_extension()->get_dynamic_partition().insert(this);
   } else {
      if (!was_static_inserted) {
         was_static_inserted = true;
         statemgr::get_physics_extension()->get_static_partition().insert(this);
      }
   }
}

void ColliderComponent::commit(Component const& from) {
   ColliderComponent const& rhs = static_cast<ColliderComponent const&>(from);
   precomputed_bounds = rhs.precomputed_bounds;
   refresh_bounds = rhs.refresh_bounds;
   mass = rhs.mass;
   velocity = rhs.velocity;
   acceleration = rhs.acceleration;
}

ColliderComponent::~ColliderComponent() {
   if (_is_static) {
      statemgr::get_physics_extension()->get_static_partition().remove(this);
   } else {
      statemgr::get_physics_extension()->get_dynamic_partition().remove(this);
   }
}