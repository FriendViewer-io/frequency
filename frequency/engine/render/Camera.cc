#include "engine/render/Camera.hh"

#include "engine/core/GObject.hh"
#include "engine/render/RenderComponent.hh"

Camera::Camera() : tracked_object(nullptr) {}

void Camera::track_object(GObject const* target) {
   tracked_object = target;
}

void Camera::update_tracking() {
   if (tracked_object != nullptr) {
      position = tracked_object->get_position();
   }
   const vec2 half_dims = dimensions * 0.5f;
   bounds.min = position - half_dims;
   bounds.max = position + half_dims;
}

vec2 Camera::get_adjusted_center(GObject const* target) const {
   return target->get_position() - bounds.min;
}

void Camera::dimensions_changed(vec2 const& new_dims) {
   dimensions = new_dims;
   update_tracking();
}

bool Camera::should_cull(RenderComponent const* rc) {
   return !rc->get_render_bounds().overlap(bounds);   
}