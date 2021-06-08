#include "engine/core/GObject.hh"

GObject::GObject() : position(), rotation(0.f), scale(1, 1), ID(0), flags(0) {}

void GObject::init(vec2 pos, float rot, vec2 scale, std::string const& name, bool disable_messaging,
                   bool active_during_pause) {
   position = pos;
   rotation = rot;
   this->scale = scale;
   this->name = name;

   ID = ID_counter++;
   flags = 0;     

   if (disable_messaging) {
      flags |= MESSAGING_DISABLE_FLAG;
   }
   if (active_during_pause) {
      flags |= PAUSE_ACTIVE_FLAG;
   }
}

void GObject::tick(float dt) {
   for (auto& comp : component_list) {
      comp->on_tick(dt);
   }
}

void GObject::post_tick(float dt) {
   for (auto& comp : component_list) {
      comp->on_post_tick(dt);
   }
}

void GObject::destroy() {
   for (auto& comp : component_list) {
      comp.reset();
   }
   component_list.clear();
}

void GObject::on_message(GObject* sender, std::string const& message) {
   if (messaging_disabled()) {
      return;
   }

   for (auto& comp : component_list) {
      comp->on_message(sender, message);
   }
}

void GObject::add_component(std::unique_ptr<Component> component) {
   component->set_parent(this);
   component_list.emplace_back(std::move(component));
}

void GObject::add_link(GObject* target, std::string const& state) {
   auto& target_list = links[state];

   target_list.push_back(target);
}

void GObject::remove_link(GObject* target, std::string const& state) {
   auto remove_list = links.find(state);

   if (remove_list == links.end()) {
      // State isn't even defined, exit
      return;
   }

   auto remove_elt = std::find(remove_list->second.begin(), remove_list->second.end(), target);
   if (remove_elt == remove_list->second.end()) {
      // GObject not found, exit
      return;
   }

   remove_list->second.erase(remove_elt);
}

void GObject::broadcast_message(std::string const& state, std::string const& message) {
   if (messaging_disabled()) {
      return;
   }
   
   auto broadcast_links = links.find(state);

   if (broadcast_links == links.end()) {
      // State isn't even defined, exit
      return;
   }

   for (GObject* obj : broadcast_links->second) {
      obj->on_message(this, message);
   }
}

void GObject::send_message(GObject* target, std::string const& state, std::string const& message) {
   if (messaging_disabled()) {
      return;
   }

   auto send_links = links.find(state);

   if (send_links == links.end()) {
      // State isn't even defined, exit
      return;
   }

   for (GObject* obj : send_links->second) {
      if (obj == target) {
         obj->on_message(this, message);
      }
   }
}

bool GObject::messaging_disabled() const {
   return (flags & MESSAGING_DISABLE_FLAG) != 0;
}

bool GObject::active_during_pause() const {
   return (flags & PAUSE_ACTIVE_FLAG) != 0;
}

void GObject::disable_messaging() {
   flags &= ~MESSAGING_DISABLE_FLAG;
}

void GObject::enable_messaging() {
   flags |= MESSAGING_DISABLE_FLAG;
}