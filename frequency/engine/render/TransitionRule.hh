#pragma once

#include <string>
#include <chrono>

class GObject;

struct TransitionRule {
   TransitionRule(int target_state) : target_state(target_state) {}
   // Called when transitioning into a new state for all rules in the new state
   virtual void on_transition() {}
   // Called when a message is received
   virtual void on_message(GObject* sender, std::string const& message) {}
   // Returns true if the transition should be taken to target_state
   virtual bool passes() = 0;

   const int target_state;
};

class MessageTransition : public TransitionRule {
public:
   MessageTransition(int target_state, std::string message)
       : TransitionRule(target_state), target_message(message), received_target_msg(false) {}

   bool passes() override;
   void on_transition() override;
   void on_message(GObject* sender, std::string const& message) override;

private:
   const std::string target_message;
   bool received_target_msg;
};

class TimerTransition : public TransitionRule {
public:
   TimerTransition(int target_state, float alarm_sec)
       : TransitionRule(target_state), alarm_sec(alarm_sec) {}

   bool passes() override;
   void on_transition() override;

private:
   const float alarm_sec;
   std::chrono::system_clock::time_point start_point;
};