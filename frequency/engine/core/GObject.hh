#pragma once

#include <map>
#include <memory>
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

   void add_component(std::unique_ptr<Component> component);
   void add_link(GObject* target, std::string const& state);
   void remove_link(GObject* target, std::string const& state);
   void remove_all_links(GObject* target);
   void broadcast_message(std::string const& state, std::string const& message);
   void send_message(GObject* target, std::string const& state, std::string const& message);

   std::string const& get_name() const { return name; }
   int get_id() const { return ID; }

   vec2 const& get_position() const { return old_data.position; }
   float get_rotation() const { return old_data.rotation; }
   vec2 const& get_scale() const { return old_data.scale; }
   Component const* get_component(std::string_view type_name) const;

   bool messaging_disabled() const;
   bool active_during_pause() const;
   bool is_munted() const;

   void disable_messaging();
   void enable_messaging();

private:
   void create_destroy_link(GObject* target);

   // External only
   GObjectInternal old_data;
   // Internal only, copied to old
   GObjectInternal new_data;

   // External only
   std::vector<std::unique_ptr<Component>> old_component_list;
   // Internal only, copied to old
   std::vector<std::unique_ptr<Component>> new_component_list;

   std::string name;
   int ID;
   std::map<std::string, std::vector<GObject*>> links;

   static constexpr uint8_t MESSAGING_DISABLE_FLAG = 0x01;
   static constexpr uint8_t PAUSE_ACTIVE_FLAG = 0x02;
   static constexpr uint8_t PENDING_DESTRUCTION_FLAG = 0x04;
   uint8_t flags;

   inline static int ID_counter = 0;
};