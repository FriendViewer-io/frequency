#pragma once

#include "engine/core/Component.hh"
#include "engine/core/ComponentGen.hh"
#include "engine/render/TransitionRule.hh"

#include <chrono>
#include <memory>
#include <string_view>

class AnimationComponent;

class AnimationController : public Component {
   DEFINE_COMPONENT_CLASS_CHUNK(AnimationController, Component, AnimationComponent)

public:
   AnimationController()
       : active_state_index(0), transitioned_this_tick(false) {}

   void on_tick(float dt) override;
   void on_message(GObject* sender, std::string const& msg) override;

   uint32_t get_component_flags() const { return Component::NO_CLONE_FLAG; }

   int add_state(std::string_view target_anim);
   void add_rule(int index, std::unique_ptr<TransitionRule> rule);

private:
   struct ACState {
      std::string target_animation;
      int state_index;
      std::vector<std::unique_ptr<TransitionRule>> transitions;
   };

   std::vector<ACState> state_map;
   int active_state_index;
   bool transitioned_this_tick;
};