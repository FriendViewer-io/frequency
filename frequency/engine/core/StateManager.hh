#pragma once

namespace statemgr {
enum class EngineState {
   INACTIVE, PAUSED, ACTIVE, SHUTDOWN
};

void set_game_state(EngineState state);
void add_extension();
void core_game_loop(float time_delta);
}