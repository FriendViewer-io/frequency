#pragma once

namespace statemgr {
enum class EngineState {
   UNINITIALIZED,
   PAUSED,
   ACTIVE,
   LEVEL_TRANSITION,
   SHUTDOWN,
};

void set_game_state(EngineState state);
void add_extension();
void core_game_loop(float time_delta);
}