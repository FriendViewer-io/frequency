#include <SDL.h>
#undef main
#include <conio.h>
#include <thread>

#include "engine/core/Component.hh"
#include "engine/core/Extension.hh"
#include "engine/core/GOList.hh"
#include "engine/core/GObject.hh"
#include "engine/core/StateManager.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/ColliderComponent.hh"
#include "engine/physics/CircleCollider.hh"
#include "engine/physics/PhysicsExtension.hh"
#include "engine/physics/Quadtree.hh"
#include <Windows.h>

void draw_circle(SDL_Renderer* renderer, float radius, float x, float y, int iterations) {
   vec2 first_point = vec2(radius, 0);
   const vec2 center = vec2(x, y);

   constexpr float total_angle = 2 * 3.141592654f;
   const float step_angle = total_angle / iterations;

   for (int i = 0; i < iterations; i++) {
      vec2 second_point = first_point.rotated(step_angle);

      vec2 fp = first_point + center;
      vec2 sp = second_point + center;

      SDL_RenderDrawLineF(renderer, fp.x, fp.y, sp.x, sp.y);
      first_point = second_point;
   }
}

void draw_vec(SDL_Renderer* r, vec2 p1, vec2 p2) {
   SDL_RenderDrawLineF(r, p1.x, p1.y, p2.x, p2.y);
}

class GhettoExtension : public Extension {
public:
   void extension_init() override {
      SDL_Init(SDL_INIT_VIDEO);
      window = SDL_CreateWindow("SampleWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000,
                                1000, SDL_WINDOW_RESIZABLE);

      renderer = SDL_CreateRenderer(window, 0, SDL_RendererFlags::SDL_RENDERER_ACCELERATED);
   }
   void pre_tick(float dt) override {
      SDL_Event evt = {0};
      SDL_PollEvent(&evt);
      if (evt.type == SDL_QUIT) {
         statemgr::shutdown_game();
      }
   }
   // Detect [& solve collisions]
   void pre_post_tick(float dt) override {}
   void tick_end(float dt) override {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
      
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      
      for (GObject* obj : *statemgr::get_object_list()) {
         auto* collider = obj->get_component("ColliderComponent");
         if (collider != nullptr) {
            CircleCollider const* cc = static_cast<CircleCollider const*>(collider);

            draw_circle(renderer, cc->get_radius(), obj->get_position().x, obj->get_position().y, 3);
         }
      }

      static_cast<Quadtree&>(statemgr::get_physics_extension()->get_spatial_partition()).for_each_level(
          [this](aabb const& aabb) {
         vec2 tr = vec2(aabb.max.x, aabb.min.y);
         vec2 bl = vec2(aabb.min.x, aabb.max.y);

         draw_vec(renderer, aabb.min, tr);
         draw_vec(renderer, tr, aabb.max);
         draw_vec(renderer, aabb.max, bl);
         draw_vec(renderer, bl, aabb.min);
      });

      SDL_RenderPresent(renderer);
   }
   void extension_reset() override {}

   std::string_view extension_name() const { return "GhettoExtension"; }

private:
   SDL_Renderer* renderer;
   SDL_Window* window;
};

int main() {
   GhettoExtension ext;

   srand(time(NULL));
   for (int i = 0; i < 100; i++) {
      statemgr::reset_game();
      GObject* n = new GObject;
      statemgr::get_object_list()->add_object(n);
      float rx = rand() % 980 + 10;
      float ry = rand() % 980 + 10;

      n->init(vec2(rx, ry), 0, vec2(1, 1), std::to_string(i), false, false);
      if (i == 0) {
         n->add_component(std::make_unique<CircleCollider>(false, vec2(20, 0), vec2(), 2));
      } else {
         n->add_component(std::make_unique<CircleCollider>(false, vec2(), vec2(), 2));
      }
   }

   statemgr::add_extension(new GhettoExtension());
   statemgr::core_game_loop(1.f / 60.f);
}