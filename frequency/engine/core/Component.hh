#pragma once

#include <memory>
#include <string>
#include <string_view>

class GObject;
struct GObjectInternal;

class Component {
public:
   static constexpr uint32_t EXECUTE_LAST_FLAG = 0x01;
   // If set, commit & clone can be ignored
   // If reset, clone & commit must be properly implemented
   static constexpr uint32_t NO_CLONE_FLAG = 0x02;

   void set_parent(GObject* parent) { this->parent = parent; }
   void set_reference_data(GObjectInternal* data) { parent_data = data; }
   GObject* get_parent() { return parent; }
   GObject const* get_parent() const { return parent; }

   // Called on GObject tick, post_tick, and on_message, only new copy
   virtual void on_tick(float dt) = 0;
   virtual void on_post_tick(float dt) const = 0;
   virtual void on_message(GObject* sender, std::string const& msg) = 0;
   // Copy new component data to old component
   virtual void commit(Component const& from) { }
   virtual std::unique_ptr<Component> clone() { return nullptr; }

   virtual std::string_view get_component_type_name() const = 0;
   virtual uint32_t get_component_flags() const { return 0; }

   virtual ~Component() {}

protected:
   GObject* parent;
   GObjectInternal* parent_data;

private:

};