#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "engine/core/Component.hh"
#include "engine/math/Vector.hh"

class GObject {
public:
   GObject();

   void init(vec2 pos, float rot, vec2 scale, std::string const& name);
   void tick(float dt);
   void post_tick(float dt);
   void destroy();
   void on_message(GObject* sender, std::string const& message);

   void add_component(std::unique_ptr<Component> component);
   void add_link(GObject* target, std::string const& state);
   void remove_link(GObject* target, std::string const& state);
   void broadcast_message(std::string const& state, std::string const& message);
   void send_message(GObject* target, std::string const& state, std::string const& message);

   std::string const& get_name() const { return name; }
   int get_id() const { return ID; }

private:
   vec2 position;
   float rotation;
   vec2 scale;
   std::string name;
   int ID;
   std::vector<std::unique_ptr<Component>> component_list;
   std::map<std::string, std::vector<GObject*>> links;

   inline static int ID_counter = 0;
};