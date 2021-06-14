#include "engine/core/StateManager.hh"

#include <chrono>
#include <thread>

#include "engine/core/Extension.hh"
#include "engine/core/GOList.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/PhysicsExtension.hh"

namespace statemgr {
PhysicsExtension* physics_ext;
EngineState game_state = EngineState::UNINITIALIZED;
GOList object_list;
std::vector<Extension*> extension_list;

EngineState get_game_state() { return game_state; }

void reset_game() { game_state = EngineState::ACTIVE; }

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

void core_game_loop(float time_delta) {
   namespace ch = std::chrono;
   using sc = std::chrono::system_clock;

   physics_ext = new PhysicsExtension(aabb(vec2(0, 0), vec2(1000, 1000)));
   add_extension(physics_ext);

   for (Extension* ext : extension_list) {
      ext->extension_init();
   }

   uint64_t tick_count = 0;
   ch::time_point start_time = sc::now();
   game_state = EngineState::ACTIVE;

   while (game_state != EngineState::SHUTDOWN) {
      // pre-tick
      for (Extension* ext : extension_list) {
         ext->pre_tick(time_delta);
      }

      object_list.for_each([time_delta](GObject* obj) {
         if (game_state == EngineState::ACTIVE ||
             (game_state == EngineState::PAUSED && obj->active_during_pause())) {
            obj->tick(time_delta);
         }
      });

      object_list.for_each([](GObject* obj) {
         if (game_state == EngineState::ACTIVE ||
             (game_state == EngineState::PAUSED && obj->active_during_pause())) {
            obj->commit();
         }
      });

      // post-tick

      for (Extension* ext : extension_list) {
         ext->pre_post_tick(time_delta);
      }

      object_list.for_each([time_delta](GObject* obj) {
         if (game_state == EngineState::ACTIVE ||
             (game_state == EngineState::PAUSED && obj->active_during_pause())) {
            obj->post_tick(time_delta);
         }
      });

      object_list.for_each([](GObject* obj) {
         if (game_state == EngineState::ACTIVE ||
             (game_state == EngineState::PAUSED && obj->active_during_pause())) {
            obj->flush_messages();
         }
      });

      // post-post-tick
      for (Extension* ext : extension_list) {
         ext->tick_end(time_delta);
      }

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

void add_extension(Extension* ext) { extension_list.emplace_back(ext); }

PhysicsExtension* get_physics_extension() { return physics_ext; }

GOList* get_object_list() { return &object_list; }

}  // namespace statemgr