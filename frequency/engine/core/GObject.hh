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

      if (flags & IN_OBJECT_LIST_FLAG) {
         return nullptr;
      }
      auto comp = std::make_unique<T>(cons...);
      T* comp_ret = comp.get();
      comp->set_parent(this);
      comp->set_reference_data(&prs);

      if (comp->get_component_flags() & Component::NO_CLONE_FLAG) {
         singular_component_list.emplace_back(std::move(comp));
      } else {
         auto old_comp = comp->clone();
         old_comp->set_parent(this);
         old_comp->set_reference_data(&pretick_prs);
         pretick_component_list.emplace_back(std::move(old_comp));
         component_list.emplace_back(std::move(comp));
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

   vec2 const& get_position() const { return prs.position; }
   float get_rotation() const { return prs.rotation; }
   vec2 const& get_scale() const { return prs.scale; }
   // Avoid frequent usage, runs a full hierarchy check
   template <typename T>
   T const* get_component() const {
      static_assert(std::is_base_of_v<Component, T>,
                    "get_component<T>() Expected a value-type class derived from Component.");
      for (auto const& comp : component_list) {
         if (comp->hierarchy_check(T::component_typename())) {
            return static_cast<T const*>(comp.get());
         }
      }
      for (auto const& comp : singular_component_list) {
         if (comp->hierarchy_check(T::component_typename())) {
            return static_cast<T const*>(comp.get());
         }
      }
      return nullptr;
   }
   // Avoid frequent usage, runs a full hierarchy check
   template <typename T>
   T* get_component() {
      static_assert(std::is_base_of_v<Component, T>,
                    "get_component<T>() Expected a value-type class derived from Component.");
      return const_cast<T*>(static_cast<GObject const*>(this)->get_component<T>());
   }

   vec2 const& get_pretick_position() const { return pretick_prs.position; }
   float get_pretick_rotation() const { return pretick_prs.rotation; }
   vec2 const& get_pretick_scale() const { return pretick_prs.scale; }
   // Avoid frequent usage, runs a full hierarchy check
   template <typename T>
   T const* get_pretick_component() const {
      static_assert(
          std::is_base_of_v<Component, T>,
          "get_pretick_component<T>() Expected a value-type class derived from Component.");
      for (auto const& comp : pretick_component_list) {
         if (comp->hierarchy_check(T::component_typename())) {
            return comp.get();
         }
      }
      for (auto const& comp : singular_component_list) {
         if (comp->hierarchy_check(T::component_typename())) {
            return comp.get();
         }
      }
      return nullptr;
   }

   bool messaging_disabled() const;
   bool active_during_pause() const;
   bool is_munted() const;
   bool in_object_list() const;

   void disable_messaging();
   void enable_messaging();

private:
   void create_destroy_link(GObject* target);
   void update_final_component_list();
   // setup dependency, run dep check, returns success
   bool added_to_object_list();

   friend class GOList;

   GObjectInternal pretick_prs;
   GObjectInternal prs;

   std::vector<std::unique_ptr<Component>> pretick_component_list;
   std::vector<std::unique_ptr<Component>> component_list;
   std::vector<std::unique_ptr<Component>> singular_component_list;
   // Reference list for components trying to execute last
   std::vector<Component*> final_component_list;
   bool final_list_invalidated = true;

   std::string name;
   int ID;
   std::map<std::string, std::vector<GObject*>> links;
   std::queue<std::pair<GObject*, std::string>> message_queue;

   static constexpr uint32_t MESSAGING_DISABLE_FLAG = 0x01;
   static constexpr uint32_t PAUSE_ACTIVE_FLAG = 0x02;
   static constexpr uint32_t PENDING_DESTRUCTION_FLAG = 0x04;
   static constexpr uint32_t IN_OBJECT_LIST_FLAG = 0x08;
   uint32_t flags;

   inline static int ID_counter = 0;
};