#include "engine/core/Component.hh"

#include "engine/core/GObject.hh"


vec2 const& Component::position() const { return parent->get_position(); }
float Component::rotation() const { return parent->get_rotation(); }
vec2 const& Component::scale() const { return parent->get_scale(); }

vec2& Component::position() { return parent_data->position; }
float& Component::rotation() { return parent_data->rotation; }
vec2& Component::scale() { return parent_data->scale; }

vec2 const& Component::pretick_position() const { return parent->get_pretick_position(); }
float Component::pretick_rotation() const { return parent->get_pretick_rotation(); }
vec2 const& Component::pretick_scale() const { return parent->get_pretick_scale(); }

void Component::add_runtime_dep(std::string_view dep_typename) {
   runtime_dependencies.push_back(std::string(dep_typename));
}

bool Component::dep_assign(std::vector<Component*>& comp_ls, std::vector<Component const*>& comp_ls_old) {
   if (runtime_dependencies.empty()) {
      return true;
   }
   for (Component* c : comp_ls) {
      auto it = std::find(runtime_dependencies.begin(), runtime_dependencies.end(), c->get_exact_typename());
      if (it == runtime_dependencies.end()) {
         return false;
      }
   }
   for (Component const* c : comp_ls_old) {
      auto it = std::find(runtime_dependencies.begin(), runtime_dependencies.end(), c->get_exact_typename());
      if (it == runtime_dependencies.end()) {
         return false;
      }
   }
   return true;
}