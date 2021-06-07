#include "engine/core/StateManager.hh"

#include "engine/core/GOList.hh"

#include <chrono>
#include <thread>

namespace statemgr {
EngineState game_state;
GOList object_list;

void set_game_state(EngineState state) { game_state = state; }

void core_game_loop(float time_delta) {
   namespace ch = std::chrono;
   using sc = std::chrono::system_clock;

   uint64_t tick_count = 0;

   ch::time_point start_time = sc::now();
   while (game_state != EngineState::SHUTDOWN) {
      // pre-tick
      
      object_list.for_each([time_delta](GObject* obj) {
         obj->tick(time_delta);
      });

      // post-tick
      
      object_list.for_each([time_delta](GObject* obj) {
         obj->post_tick(time_delta);
      });
      
      // post-post-tick

      tick_count++;
      ch::time_point next_tick_start = start_time + ch::milliseconds(static_cast<uint64_t>(tick_count * time_delta));
      std::this_thread::sleep_until(next_tick_start);
   } // end while
}

}  // namespace statemgr