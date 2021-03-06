#pragma once

#include <functional>
#include <memory>
#include <vector>

class GOList;
class Extension;
class PhysicsExtension;
class RenderExtension;
class InputExtension;

namespace statemgr {
enum class EngineState {
   UNINITIALIZED,
   PAUSED,
   ACTIVE,
   LEVEL_TRANSITION,
   SHUTDOWN,
};

EngineState get_game_state();
void reset_game();
void shutdown_game();
void pause_game();
void resume_game();
void level_change(/* specify level?? */);
void no_init();

void core_game_loop(float time_delta, std::function<void()> const& post_extension_init);
void add_extension(Extension* ext);
PhysicsExtension* get_physics_extension();
RenderExtension* get_render_extension();
InputExtension* get_input_extension();
std::vector<Extension*>& get_extension_list();
GOList* get_object_list();

}  // namespace statemgr