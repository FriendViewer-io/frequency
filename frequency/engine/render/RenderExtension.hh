#pragma once

#include <array>
#include <memory>
#include <vector>

#include "engine/core/Extension.hh"

struct SDL_Renderer;
struct SDL_Window;
class Camera;
class GObject;
class RenderComponent;

class RenderExtension : public Extension {
public:
   RenderExtension();

   // Create window
   void extension_init() override;

   // Update window events
   void pre_tick(float dt) override;
   void pre_post_tick(float dt) override {}

   // Render all GObjects that have a RenderComponent
   void tick_end(float dt) override;
   void extension_reset() override {}

   std::string_view extension_name() const { return "RenderExtension"; }

   int window_width() const;
   int window_height() const;
   void window_resize(int width, int height);

   void object_added(GObject* obj) override;
   void object_removed(GObject* obj) override;

   Camera* get_camera() { return camera.get(); }
   Camera const* get_camera() const { return camera.get(); }

private:
   SDL_Renderer* renderer;
   SDL_Window* window;
   void* gl_ctx;
   std::unique_ptr<Camera> camera;

   std::array<std::vector<RenderComponent*>, 5> render_buckets;
};