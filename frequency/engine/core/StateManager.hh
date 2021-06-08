#pragma once

class GOList;

namespace statemgr {
enum class EngineState {
   UNINITIALIZED,
   PAUSED,
   ACTIVE,
   LEVEL_TRANSITION,
   SHUTDOWN,
};

EngineState get_game_state();
void shutdown_game();
void pause_game();
void resume_game();
void level_change(/* specify level?? */);

void add_extension();
void core_game_loop(float time_delta);
GOList* get_object_list();

}