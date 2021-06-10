#pragma once

#include <memory>
#include <string>
#include <string_view>

class GObject;
struct GObjectInternal;

class Component {
public:
   void set_parent(GObject* parent) { this->parent = parent; }
   void set_reference_data(GObjectInternal* data) { parent_data = data; }

   // Called on GObject tick, post_tick, and on_message, only new copy
   virtual void on_tick(float dt) = 0;
   virtual void on_post_tick(float dt) = 0;
   virtual void on_message(GObject* sender, std::string const& msg) = 0;

   // Copy new component data to old component
   virtual void commit(Component const& from) = 0;
   virtual std::unique_ptr<Component> clone() = 0;

   virtual std::string_view get_component_type_name() const = 0;

   virtual ~Component() {}

protected:
   GObject* parent;
   GObjectInternal* parent_data;

private:

};