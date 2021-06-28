#include "engine/render/TransitionRule.hh"

#include "engine/core/GObject.hh"

bool MessageTransition::passes() {
   return received_target_msg;
}

void MessageTransition::on_transition() {
   received_target_msg = false;
}

void MessageTransition::on_message(GObject* sender, std::string const& message) {
   if (message == target_message) {
      received_target_msg = true;
   }
}



bool TimerTransition::passes() {
   const auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_point);
   const float elapsed_sec = static_cast<float>(elapsed.count()) / 1000000.f;

   return elapsed_sec >= alarm_sec;
}

void TimerTransition::on_transition() {
   start_point = std::chrono::system_clock::now();
}