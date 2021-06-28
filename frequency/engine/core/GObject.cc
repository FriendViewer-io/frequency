#include "engine/core/GObject.hh"

GObjectInternal::GObjectInternal() : position(0, 0), rotation(0), scale(1, 1) {}

GObject::GObject() : ID(0), flags(0) {}

GObject::~GObject() {
   enable_messaging();
   broadcast_message("$$destroy", "$$destroy");
}

void GObject::init(vec2 pos, float rot, vec2 scale, std::string const& name, bool disable_messaging,
                   bool active_during_pause) {
   if (flags & IN_OBJECT_LIST_FLAG) {
      return;
   }

   prs.position = pos;
   pretick_prs.position = pos;

   prs.rotation = rot;
   pretick_prs.rotation = rot;

   prs.scale = scale;
   pretick_prs.scale = scale;

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

void GObject::update_final_component_list() {
   if (final_list_invalidated) {
      final_component_list.clear();
      for (auto& comp : component_list) {
         if (comp->get_component_flags() & Component::EXECUTE_LAST_FLAG) {
            final_component_list.push_back(comp.get());
         }
      }
      for (auto& comp : singular_component_list) {
         if (comp->get_component_flags() & Component::EXECUTE_LAST_FLAG) {
            final_component_list.push_back(comp.get());
         }
      }
      final_list_invalidated = false;
   }
}

void GObject::tick(float dt) {
   update_final_component_list();
   for (auto& comp : component_list) {
      if (!(comp->get_component_flags() & Component::EXECUTE_LAST_FLAG)) {
         comp->on_tick(dt);
      }
   }
   for (auto& comp : singular_component_list) {
      if (!(comp->get_component_flags() & Component::EXECUTE_LAST_FLAG)) {
         comp->on_tick(dt);
      }
   }
   for (auto& comp : final_component_list) {
      comp->on_tick(dt);
   }
}

void GObject::commit() {
   pretick_prs = prs;
   for (int i = 0; i < component_list.size(); i++) {
      pretick_component_list[i]->commit(*component_list[i]);
   }
}

void GObject::post_tick(float dt) {
   update_final_component_list();
   for (auto& comp : component_list) {
      if (!(comp->get_component_flags() & Component::EXECUTE_LAST_FLAG)) {
         comp->on_post_tick(dt);
      }
   }
   for (auto& comp : singular_component_list) {
      if (!(comp->get_component_flags() & Component::EXECUTE_LAST_FLAG)) {
         comp->on_post_tick(dt);
      }
   }
   for (auto& comp : final_component_list) {
      comp->on_post_tick(dt);
   }
}

void GObject::munt() { flags |= PENDING_DESTRUCTION_FLAG; }

void GObject::on_message(GObject* sender, std::string const& message) {
   if (messaging_disabled()) {
      if (message == "$$destroy") {
         remove_all_links(sender);
      }
      return;
   }

   update_final_component_list();
   for (auto& comp : component_list) {
      if (!(comp->get_component_flags() & Component::EXECUTE_LAST_FLAG)) {
         comp->on_message(sender, message);
      }
   }
   for (auto& comp : singular_component_list) {
      if (!(comp->get_component_flags() & Component::EXECUTE_LAST_FLAG)) {
         comp->on_message(sender, message);
      }
   }
   for (auto& comp : final_component_list) {
      comp->on_message(sender, message);
   }

   if (message == "$$destroy") {
      remove_all_links(sender);
   }
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

   if (target == this) {
      enqueue_message(this, message);
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

bool GObject::messaging_disabled() const { return (flags & MESSAGING_DISABLE_FLAG) != 0; }

bool GObject::active_during_pause() const { return (flags & PAUSE_ACTIVE_FLAG) != 0; }

bool GObject::is_munted() const { return (flags & PENDING_DESTRUCTION_FLAG) != 0; }

bool GObject::in_object_list() const { return (flags & IN_OBJECT_LIST_FLAG) != 0; }

void GObject::disable_messaging() { flags |= MESSAGING_DISABLE_FLAG; }

void GObject::enable_messaging() { flags &= ~MESSAGING_DISABLE_FLAG; }

bool GObject::added_to_object_list() {
   std::vector<Component*> deps;
   std::vector<Component const*> pretick_deps;

   for (auto& comp : component_list) {
      deps.push_back(comp.get());
   }
   for (auto& comp : pretick_component_list) {
      pretick_deps.push_back(comp.get());
   }
   for (auto& comp : singular_component_list) {
      deps.push_back(comp.get());
      pretick_deps.push_back(comp.get());
   }

   for (auto& comp : component_list) {
      if (!comp->dep_assign(deps, pretick_deps)) {
         return false;
      }
   }
   for (auto& comp : singular_component_list) {
      if (!comp->dep_assign(deps, pretick_deps)) {
         return false;
      }
   }

   flags |= IN_OBJECT_LIST_FLAG;
   return true;
}
