#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "engine/math/Vector.hh"


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
   vec2 const& position() const;
   float rotation() const;
   vec2 const& scale() const;
   vec2& position();
   float& rotation();
   vec2& scale();
   vec2 const& pretick_position() const;
   float pretick_rotation() const;
   vec2 const& pretick_scale() const;

   template <typename T>
   void add_runtime_dep() {
      static_assert(std::is_base_of_v<Component, T>, "add_runtime_dep only supports types derived from Component!");
      runtime_dependencies.push_back(std::string(T::component_typename()));
   }

   void add_runtime_dep(std::string_view dep_typename);

   //////////////////////////////////////////////////////////
   // DO NOT MANUALLY OVERRIDE, LET GENERATOR HANDLE THESE //
   //////////////////////////////////////////////////////////

   constexpr static std::string_view component_typename() { return "Component"; }
   virtual std::string_view get_exact_typename() const { return Component::component_typename(); }
   virtual bool hierarchy_check(std::string_view cname) const {
      return cname == Component::component_typename();
   }
   virtual bool dep_assign(std::vector<Component*>& comp_ls,
                           std::vector<Component const*>& comp_ls_old);

   //////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////

   // Called on GObject tick, post_tick, and on_message, only new copy
   virtual void on_tick(float dt) {}
   virtual void on_post_tick(float dt) const {}
   virtual void on_message(GObject* sender, std::string const& msg) {}
   // Copy new component data to old component
   virtual void commit(Component const& from) {}
   virtual std::unique_ptr<Component> clone() { return nullptr; }

   virtual uint32_t get_component_flags() const { return 0; }

   virtual ~Component() {}

protected:
   GObject* parent;
   GObjectInternal* parent_data;

private:
   std::vector<std::string> runtime_dependencies;
};