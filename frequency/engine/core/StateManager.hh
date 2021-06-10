#pragma once

#include <memory>

class GOList;
class Extension;

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

void core_game_loop(float time_delta);
void add_extension(Extension* ext);
GOList* get_object_list();

}