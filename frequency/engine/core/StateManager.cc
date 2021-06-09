#include "engine/core/StateManager.hh"

#include <chrono>
#include <thread>

#include "engine/core/GOList.hh"

namespace statemgr {
EngineState game_state = EngineState::UNINITIALIZED;
GOList object_list;

EngineState get_game_state(EngineState state) { return game_state; }

void shutdown_game() {
   if (game_state == EngineState::ACTIVE || game_state == EngineState::LEVEL_TRANSITION ||
       game_state == EngineState::PAUSED) {
      game_state = EngineState::SHUTDOWN;
   } else {
      // log warning?
   }
}

void pause_game() {
   if (game_state == EngineState::ACTIVE) {
      game_state = EngineState::PAUSED;
   } else {
      // log warning?
   }
}

void resume_game() {
   if (game_state == EngineState::PAUSED) {
      game_state = EngineState::ACTIVE;
   } else {
      // log warning?
   }
}

void level_change() {
   if (game_state == EngineState::ACTIVE) {
      game_state = EngineState::LEVEL_TRANSITION;
   } else if (game_state == EngineState::PAUSED) {
      // save level for later when game is unpaused
   } else {
      // log warning?
   }
}

GOList* get_object_list() { return &object_list; }

void core_game_loop(float time_delta) {
   namespace ch = std::chrono;
   using sc = std::chrono::system_clock;

   uint64_t tick_count = 0;

   ch::time_point start_time = sc::now();
   game_state = EngineState::ACTIVE;

   while (game_state != EngineState::SHUTDOWN) {
      // pre-tick
      if (game_state == EngineState::ACTIVE) {
         object_list.for_each([time_delta](GObject* obj) { obj->tick(time_delta); });

         object_list.for_each([](GObject* obj) { obj->commit(); });

         // post-tick

         object_list.for_each([time_delta](GObject* obj) { obj->post_tick(time_delta); });
      } else if (game_state == EngineState::PAUSED) {
         // do only some of them
         object_list.for_each([time_delta](GObject* obj) {
            if (obj->active_during_pause()) {
               obj->tick(time_delta);
            }
         });

         object_list.for_each([](GObject* obj) {
            if (obj->active_during_pause()) {
               obj->commit();
            }
         });
         
         // post-tick

         object_list.for_each([time_delta](GObject* obj) {
            if (obj->active_during_pause()) {
               obj->post_tick(time_delta);
            }
         });
      }
      // post-post-tick

      // loop through object list, delete all components marked as munted
      object_list.remove_if([](GObject* obj) {
         if (obj->is_munted()) {
            delete obj;
            return true;
         }
         return false;
      });

      tick_count++;
      ch::time_point next_tick_start =
          start_time + ch::milliseconds(static_cast<uint64_t>(tick_count * time_delta * 1000.f));
      std::this_thread::sleep_until(next_tick_start);
   }  // end while
}

}  // namespace statemgr