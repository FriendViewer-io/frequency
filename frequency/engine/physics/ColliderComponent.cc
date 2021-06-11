#include "engine/physics/ColliderComponent.hh"

#include "engine/core/GObject.hh"

// new position = 1/2(accel * time^2) + (initial v * time) + initial position
// new velocity = accel * time + initial velocity



void ColliderComponent::on_tick(float dt) {
   parent_data->position = (acceleration * dt * dt) * 0.5f + (velocity * dt) + parent_data->position;
   velocity = (acceleration * dt) + velocity;
   printf("My current position is %f %f\n", parent_data->position.x, parent_data->position.y);
}

void ColliderComponent::commit(Component const& from) {
   ColliderComponent const& rhs = static_cast<ColliderComponent const&>(from);
   mass = rhs.mass;
   velocity = rhs.velocity;
   acceleration = rhs.acceleration;
}