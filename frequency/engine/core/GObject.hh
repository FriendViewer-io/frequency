#pragma once

#include <map>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "engine/core/Component.hh"
#include "engine/math/Vector.hh"

struct GObjectInternal {
   GObjectInternal();

   vec2 position;
   float rotation;
   vec2 scale;
};

class GObject {
public:
   GObject();
   ~GObject();

   void init(vec2 pos, float rot, vec2 scale, std::string const& name, bool disable_messaging,
             bool active_during_pause);
   void tick(float dt);
   void commit();
   void post_tick(float dt);
   // Meaning: request to destroy at end of tick
   void munt();
   void on_message(GObject* sender, std::string const& message);

   template <typename T, typename... Args>
   T* create_component(Args... cons) {
      static_assert(std::is_base_of_v<Component, T>,
                    "create_component can only construct types derived from Component!");
      auto comp = std::make_unique<T>(cons...);
      T* comp_ret = comp.get();
      comp->set_parent(this);
      comp->set_reference_data(&new_data);

      if (comp->get_component_flags() & Component::NO_CLONE_FLAG) {
         singular_component_list.emplace_back(std::move(comp));
      } else {
         auto old_comp = comp->clone();
         old_comp->set_parent(this);
         old_comp->set_reference_data(&old_data);
         old_component_list.emplace_back(std::move(old_comp));
         new_component_list.emplace_back(std::move(comp));
      }
      final_list_invalidated = true;
      return comp_ret;
   }

   void add_link(GObject* target, std::string const& state);
   void remove_link(GObject* target, std::string const& state);
   void remove_all_links(GObject* target);
   void enqueue_message(GObject* sender, std::string const& message);
   void flush_messages();
   void broadcast_message(std::string const& state, std::string const& message);
   void send_message(GObject* target, std::string const& state, std::string const& message);

   std::string const& get_name() const { return name; }
   int get_id() const { return ID; }

   vec2 const& get_position() const { return old_data.position; }
   float get_rotation() const { return old_data.rotation; }
   vec2 const& get_scale() const { return old_data.scale; }
   Component const* get_component(std::string_view type_name) const;

   vec2 const& get_staging_position() const { return new_data.position; }
   float get_staging_rotation() const { return new_data.rotation; }
   vec2 const& get_staging_scale() const { return new_data.scale; }
   Component const* get_staging_component(std::string_view type_name) const;
   Component* get_staging_component(std::string_view type_name);

   bool messaging_disabled() const;
   bool active_during_pause() const;
   bool is_munted() const;

   void disable_messaging();
   void enable_messaging();

private:
   void create_destroy_link(GObject* target);
   void update_final_component_list();

   // External only
   GObjectInternal old_data;
   // Internal only, copied to old
   GObjectInternal new_data;

   // External only
   std::vector<std::unique_ptr<Component>> old_component_list;
   // Internal only, copied to old
   std::vector<std::unique_ptr<Component>> new_component_list;
   // Both internal & external
   std::vector<std::unique_ptr<Component>> singular_component_list;
   // Reference list for components trying to execute last
   std::vector<Component*> final_component_list;
   bool final_list_invalidated = true;

   std::string name;
   int ID;
   std::map<std::string, std::vector<GObject*>> links;
   std::queue<std::pair<GObject*, std::string>> message_queue;

   static constexpr uint8_t MESSAGING_DISABLE_FLAG = 0x01;
   static constexpr uint8_t PAUSE_ACTIVE_FLAG = 0x02;
   static constexpr uint8_t PENDING_DESTRUCTION_FLAG = 0x04;
   uint8_t flags;

   inline static int ID_counter = 0;
};