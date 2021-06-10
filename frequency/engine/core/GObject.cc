#include "engine/core/GObject.hh"

GObjectInternal::GObjectInternal() : position(0, 0), rotation(0), scale(1, 1) {}

GObject::GObject() : old_data(), new_data(), ID(0), flags(0) {}

GObject::~GObject() {
   enable_messaging();
   broadcast_message("$$destroy", "$$destroy");
}

void GObject::init(vec2 pos, float rot, vec2 scale, std::string const& name, bool disable_messaging,
                   bool active_during_pause) {
   new_data.position = pos;
   old_data.position = pos;

   new_data.rotation = rot;
   old_data.rotation = rot;

   new_data.scale = scale;
   old_data.scale = scale;

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
   for (auto& comp : new_component_list) {
      comp->on_tick(dt);
   }
}

void GObject::commit() {
   old_data = new_data;
   for (int i = 0; i < new_component_list.size(); i++) {
      old_component_list[i]->commit(*new_component_list[i]);
   }
}

void GObject::post_tick(float dt) {
   for (auto& comp : new_component_list) {
      comp->on_post_tick(dt);
   }
}

void GObject::munt() {
   flags |= PENDING_DESTRUCTION_FLAG;
}

void GObject::on_message(GObject* sender, std::string const& message) {
   if (messaging_disabled()) {
      if (message == "$$destroy") {
         remove_all_links(sender);
      }
      return;
   }

   for (auto& comp : new_component_list) {
      comp->on_message(sender, message);
   }

   if (message == "$$destroy") {
      remove_all_links(sender);
   }
}

void GObject::add_component(std::unique_ptr<Component> component) {
   component->set_parent(this);
   component->set_reference_data(&new_data);
   
   old_component_list.emplace_back(component->clone());
   new_component_list.emplace_back(std::move(component));
}

void GObject::add_link(GObject* target, std::string const& state) {
   auto& target_list = links[state];
   target_list.push_back(target);

   create_destroy_link(target);
}

void GObject::create_destroy_link(GObject* target) {
   auto& destroy_list = links["$$destroy"];

   for (GObject* link : destroy_list) {
      if (link == target) {
         return;
      }
   }
   destroy_list.push_back(target);
   // if link already exists, don't call
   target->create_destroy_link(this);
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

void GObject::remove_all_links(GObject* target) {
   for (auto&& pair : links) {
      remove_link(target, pair.first);
   }
}

void GObject::enqueue_message(GObject* sender, std::string const& message) {
   message_queue.emplace(sender, message);
}

void GObject::flush_messages() {
   while (!message_queue.empty()) {
      on_message(message_queue.front().first, message_queue.front().second);
      message_queue.pop();
   }
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
      obj->enqueue_message(this, message);
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
         obj->enqueue_message(this, message);
      }
   }
}

Component const* GObject::get_component(std::string_view type_name) const {
   for (auto const& comp : old_component_list) {
      if (comp->get_component_type_name() == type_name) {
         return comp.get();
      }
   }
   return nullptr;
}

bool GObject::messaging_disabled() const { return (flags & MESSAGING_DISABLE_FLAG) != 0; }

bool GObject::active_during_pause() const { return (flags & PAUSE_ACTIVE_FLAG) != 0; }

bool GObject::is_munted() const { return (flags & PENDING_DESTRUCTION_FLAG) != 0; }

void GObject::disable_messaging() { flags |= MESSAGING_DISABLE_FLAG; }

void GObject::enable_messaging() { flags &= ~MESSAGING_DISABLE_FLAG; }