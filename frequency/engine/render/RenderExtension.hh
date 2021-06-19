#pragma once

#include "engine/core/Extension.hh"

struct SDL_Renderer;
struct SDL_Window;

class RenderExtension : public Extension {
public:
   RenderExtension() {}

   // Create window
   void extension_init() override;

   // Update window events
   void pre_tick(float dt) override;

   void pre_post_tick(float dt) override {}

   // Render all GObjects that have a RenderComponent
   void tick_end(float dt) override;

   void extension_reset() override {}

   std::string_view extension_name() const { return "RenderExtension"; }

private:
   SDL_Renderer* renderer;
   SDL_Window* window;
   void* gl_ctx;
};