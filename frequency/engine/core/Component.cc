#include "engine/core/Component.hh"

#include "engine/core/GObject.hh"


vec2 const& Component::position() const { return parent->get_position(); }
float Component::rotation() const { return parent->get_rotation(); }
vec2 const& Component::scale() const { return parent->get_scale(); }

vec2& Component::position() { return parent_data->position; }
float& Component::rotation() { return parent_data->rotation; }
vec2& Component::scale() { return parent_data->scale; }

vec2 const& Component::pretick_position() const {
   return parent->get_pretick_position();
}
float Component::pretick_rotation() const {
   return parent->get_pretick_rotation();
}
vec2 const& Component::pretick_scale() const {
   return parent->get_pretick_scale();
}