#include "engine/render/AnimationController.hh"

#include "engine/core/GObject.hh"
#include "engine/render/AnimationComponent.hh"

DEFINE_COMPONENT_GLOBAL_CHUNK(AnimationController)

void AnimationController::on_tick(float dt) {
   transitioned_this_tick = false;
   for (auto& tr : state_map[active_state_index].transitions) {
      if (tr->passes()) {
         active_state_index = tr->target_state;
         transitioned_this_tick = true;
         break;
      }
   }

   if (transitioned_this_tick) {
      get<AnimationComponent>(this)->set_animation(state_map[active_state_index].target_animation);
      for (auto& tr : state_map[active_state_index].transitions) {
         tr->on_transition();
      }
   }
}

void AnimationController::on_message(GObject* sender, std::string const& msg) {
   for (auto& tr : state_map[active_state_index].transitions) {
      tr->on_message(sender, msg);
   }
}

int AnimationController::add_state(std::string_view target_anim) {
   state_map.emplace_back(ACState {std::string(target_anim), static_cast<int>(state_map.size()), {}});
   return state_map.back().state_index;
}

void AnimationController::add_rule(int index, std::unique_ptr<TransitionRule> rule) {
   state_map[index].transitions.emplace_back(std::move(rule));
}