#include <SDL.h>
#undef main
#include <conio.h>

#include "engine/core/Component.hh"
#include "engine/core/Extension.hh"
#include "engine/core/GOList.hh"
#include "engine/core/GObject.hh"
#include "engine/core/StateManager.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/ColliderComponent.hh"
#include "engine/physics/CircleCollider.hh"
#include "engine/physics/PhysicsExtension.hh"

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

class GhettoExtension : public Extension {
public:
   void extension_init() override {
      SDL_Init(SDL_INIT_VIDEO);
      window = SDL_CreateWindow("SampleWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500,
                                500, SDL_WINDOW_RESIZABLE);

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

            draw_circle(renderer, cc->get_radius(), obj->get_position().x, obj->get_position().y, 50);
         }
      }

      SDL_RenderPresent(renderer);
   }
   void extension_reset() override {}

   std::string_view extension_name() const { return "GhettoExtension"; }

private:
   SDL_Renderer* renderer;
   SDL_Window* window;
};

int main() {
   statemgr::add_extension(new PhysicsExtension());
   statemgr::add_extension(new GhettoExtension());

   GOList* object_list = statemgr::get_object_list();

   GObject* ob1 = new GObject, *ob2 = new GObject;

   // testing disable_messaging
   ob1->init(vec2(195, 100), 0, vec2(), "adyam", false, false);
   ob2->init(vec2(300, 100), 0, vec2(), "joe", false, false);
   ob1->add_component(std::make_unique<CircleCollider>(vec2(1, 0), vec2(0, 0), 50));
   ob2->add_component(std::make_unique<CircleCollider>(vec2(0, 0), vec2(0, 0), 50));
   object_list->add_object(ob1);
   object_list->add_object(ob2);

   statemgr::core_game_loop(1.f / 60.f);
}