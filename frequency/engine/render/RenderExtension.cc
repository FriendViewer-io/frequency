#include "engine/render/RenderExtension.hh"

#include "engine/core/GOList.hh"
#include "engine/core/GObject.hh"
#include "engine/core/StateManager.hh"
#include "engine/render/Camera.hh"
#include "engine/render/RenderComponent.hh"


// clang-format off
#include <gl/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
// clang-format on

RenderExtension::RenderExtension()
    : renderer(nullptr), window(nullptr), gl_ctx(nullptr), camera(nullptr) {}

void RenderExtension::extension_init() {
   camera = std::make_unique<Camera>();

   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      return;
   }
   window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800,
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

   glViewport(0, 0, 800, 800);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, 800, 800, 0, -1, 1);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   camera->dimensions_changed(vec2(800, 800));
}

void RenderExtension::pre_tick(float dt) {
   SDL_Event evt = {0};
   SDL_PollEvent(&evt);
   if (evt.type == SDL_QUIT) {
      statemgr::shutdown_game();
   } else if (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
      window_resize(evt.window.data1, evt.window.data2);
      camera->dimensions_changed(vec2(evt.window.data1, evt.window.data2));
   }
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);
}

void RenderExtension::tick_end(float dt) {
   camera->update_tracking();

   std::vector<RenderComponent*> reorder_comp;

   for (int i = 0; i < 5; i++) {
      for (int j = 0; j < render_buckets[i].size(); j++) {
         RenderComponent* rc = render_buckets[i][j];

         if (!camera->should_cull(rc)) {
            rc->bind_data(get_camera());
            glDrawElements(GL_TRIANGLES, rc->get_draw_count(), GL_UNSIGNED_INT, 0);
         }

         const int bucket = static_cast<int>(rc->get_sort_group());
         if (bucket != i) {
            render_buckets[i].erase(render_buckets[i].begin() + j);
            j--;
            reorder_comp.push_back(rc);
         }
      }
   }

   for (RenderComponent* rc : reorder_comp) {
      const int bucket = static_cast<int>(rc->get_sort_group());
      render_buckets[bucket].push_back(rc);
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

void RenderExtension::object_added(GObject* obj) {
   RenderComponent* rc = obj->get_component<RenderComponent>();
   if (rc != nullptr) {
      int bucket = static_cast<int>(rc->get_sort_group());
      render_buckets[bucket].push_back(rc);
   }
}

void RenderExtension::object_removed(GObject* obj) {
   RenderComponent* rc = obj->get_component<RenderComponent>();
   if (rc != nullptr) {
      int bucket_idx = static_cast<int>(rc->get_sort_group());
      auto it = std::find(render_buckets[bucket_idx].begin(), render_buckets[bucket_idx].end(), rc);
      if (it != render_buckets[bucket_idx].end()) {
         render_buckets[bucket_idx].erase(it);
      } else {
         for (auto& bucket : render_buckets) {
            auto it = std::find(bucket.begin(), bucket.end(), rc);
            if (it != bucket.end()) {
               bucket.erase(it);
               break;
            }
         }
      }
   }
}

void RenderExtension::window_resize(int width, int height) {
   glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, width, height, 0, -1, 1);
}