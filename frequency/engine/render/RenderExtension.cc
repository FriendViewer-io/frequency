#include "engine/render/RenderExtension.hh"

// clang-format off
#include <gl/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
// clang-format on

#include "engine/core/GOList.hh"
#include "engine/core/GObject.hh"
#include "engine/core/StateManager.hh"
#include "engine/render/RenderComponent.hh"

void RenderExtension::extension_init() {
   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      return;
   }
   window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 2000, 1000,
                             SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

   gl_ctx = SDL_GL_CreateContext(window);

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

   glewExperimental = GL_TRUE;
   glewInit();

   glEnable(GL_TEXTURE_2D);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glViewport(0, 0, 2000, 1000);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, 2000, 1000, 0, -1, 1);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void RenderExtension::pre_tick(float dt) {
   SDL_Event evt = {0};
   SDL_PollEvent(&evt);
   if (evt.type == SDL_QUIT) {
      statemgr::shutdown_game();
   } else if (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
      window_resize(evt.window.data1, evt.window.data2);
   }
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);
}

void RenderExtension::tick_end(float dt) {
   for (GObject* go : *statemgr::get_object_list()) {
      RenderComponent const* rc =
          static_cast<RenderComponent const*>(go->get_component("RenderComponent"));
      if (rc != nullptr) {
         rc->bind_data();
         glDrawElements(GL_TRIANGLES, rc->get_draw_count(), GL_UNSIGNED_INT, 0);
      }
   }

   SDL_GL_SwapWindow(window);
}

int RenderExtension::window_width() const {
   int w;
   SDL_GetWindowSize(window, &w, nullptr);
   return w;
}

int RenderExtension::window_height() const {
   int h;
   SDL_GetWindowSize(window, nullptr, &h);
   return h;
}

void RenderExtension::window_resize(int width, int height) {
   glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, width, height, 0, -1, 1);
}