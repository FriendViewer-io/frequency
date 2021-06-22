#include "engine/physics/ColliderComponent.hh"

#include "engine/core/GObject.hh"
#include "engine/core/StateManager.hh"
#include "engine/physics/PhysicsExtension.hh"
#include "engine/physics/SpatialPartition.hh"

void ColliderComponent::on_tick(float dt) {
   if (refresh_bounds) {
      refresh_bounds = false;
      update_bounds();
      update_centroid();
   }
   const vec2 local_gravity = statemgr::get_physics_extension()->get_gravity() * gravity_scalar;

   if (!_is_static) {
      // remove and reinsert
      aabb old_bbox = bounding_box();
      old_bbox.shift(get_parent()->get_position());
      statemgr::get_physics_extension()->get_dynamic_partition().remove(old_bbox, this);
      // insert & remove from cell-based partition
      parent_data->position = ((acceleration + local_gravity) * dt * dt) * 0.5f + (velocity * dt) +
                              parent_data->position;
      velocity = ((acceleration + local_gravity) * dt) + velocity;

      aabb new_bbox = bounding_box();
      new_bbox.shift(parent_data->position);
      statemgr::get_physics_extension()->get_dynamic_partition().insert(new_bbox, this);
   } else {
      if (!was_static_inserted) {
         was_static_inserted = true;
         aabb bbox = bounding_box();
         bbox.shift(parent_data->position);
         statemgr::get_physics_extension()->get_static_partition().insert(bbox, this);
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

void ColliderComponent::push_object(vec2 direction) {
   aabb bbox = bounding_box();
   bbox.shift(get_parent()->get_position());
   if (_is_static) {
      statemgr::get_physics_extension()->get_static_partition().remove(bbox, this);
   } else {
      statemgr::get_physics_extension()->get_dynamic_partition().remove(bbox, this);
   }

   parent_data->position += direction;

   if (_is_static) {
      statemgr::get_physics_extension()->get_static_partition().insert(bbox, this);
   } else {
      statemgr::get_physics_extension()->get_dynamic_partition().insert(bbox, this);
   }
}

ColliderComponent::~ColliderComponent() {
   aabb bbox = bounding_box();
   bbox.shift(get_parent()->get_position());
   if (_is_static) {
      statemgr::get_physics_extension()->get_static_partition().remove(bbox, this);
   } else {
      statemgr::get_physics_extension()->get_dynamic_partition().remove(bbox, this);
   }
}