#include "engine/core/StateManager.hh"

#include <chrono>
#include <thread>

#include "engine/core/Extension.hh"
#include "engine/core/GOList.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/PhysicsExtension.hh"
#include "engine/render/RenderExtension.hh"

namespace statemgr {
PhysicsExtension* physics_ext;
RenderExtension* render_ext;
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

void no_init() {}

void core_game_loop(float time_delta, std::function<void()> const& post_extension_init) {
   namespace ch = std::chrono;
   using sc = std::chrono::system_clock;

   physics_ext = new PhysicsExtension(aabb(vec2(0, 0), vec2(1000, 1000)), vec2(100, 100));
   add_extension(physics_ext);

   render_ext = new RenderExtension();
   add_extension(render_ext);

   for (Extension* ext : extension_list) {
      ext->extension_init();
   }

   post_extension_init();

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

      const ch::time_point cur_time = sc::now();
      const ch::milliseconds skip_time_cutoff = ch::milliseconds(static_cast<uint64_t>(10 * time_delta * 1000.f));
      const float us_per_tick = time_delta * 1000000.f;
      tick_count++;

      ch::time_point next_tick_start =
          start_time + ch::milliseconds(static_cast<uint64_t>(tick_count * time_delta * 1000.f));

      if (cur_time - next_tick_start > skip_time_cutoff) {
         auto new_tick_elapsed = ch::duration_cast<std::chrono::microseconds>(cur_time - start_time).count() / us_per_tick;
         tick_count = static_cast<uint64_t>(new_tick_elapsed);
      }
      std::this_thread::sleep_until(next_tick_start);
   }  // end while
}

void add_extension(Extension* ext) { extension_list.emplace_back(ext); }

PhysicsExtension* get_physics_extension() { return physics_ext; }
RenderExtension* get_render_extension() { return render_ext; }

GOList* get_object_list() { return &object_list; }

}  // namespace statemgr