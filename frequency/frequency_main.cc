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
#include "engine/physics/CellPartition.hh"
#include <Windows.h>


void draw_vec(SDL_Renderer* r, vec2 p1, vec2 p2) {
   SDL_RenderDrawLineF(r, p1.x, p1.y, p2.x, p2.y);
}

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

void draw_box(SDL_Renderer* renderer, aabb const& bounds) {
   vec2 tr = vec2(bounds.max.x, bounds.min.y);
   vec2 bl = vec2(bounds.min.x, bounds.max.y);

   draw_vec(renderer, bounds.min, tr);
   draw_vec(renderer, tr, bounds.max);
   draw_vec(renderer, bounds.max, bl);
   draw_vec(renderer, bl, bounds.min);
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
      
      auto& quad_part = static_cast<Quadtree&>(statemgr::get_physics_extension()->get_static_partition());
      auto& cell_part = static_cast<CellPartition&>(statemgr::get_physics_extension()->get_dynamic_partition());

      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      quad_part.for_each_level(
          [this](aabb const& aabb) {
         draw_box(renderer, aabb);
      });

      aabb world_bounds = cell_part.get_world_bounds();
      vec2 cell_dimensions = cell_part.get_cell_dimensions();

      int num_cells_width =
       static_cast<int>(ceilf((world_bounds.max.x - world_bounds.min.x) / cell_dimensions.x));
      int num_cells_height =
       static_cast<int>(ceilf((world_bounds.max.y - world_bounds.min.y) / cell_dimensions.y));

      SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
      for (int i = 0; i < num_cells_height; i++) {
         for (int j = 0; j < num_cells_width; j++) {
            vec2 cell_top_left = world_bounds.min + vec2(0, cell_dimensions.y * i) + vec2(cell_dimensions.x * j, 0);
            vec2 cell_top_right = cell_top_left + vec2(cell_dimensions.x, 0);
            vec2 cell_bottom_left = cell_top_left + vec2(0, cell_dimensions.y);

            draw_vec(renderer, cell_top_left, cell_top_right);
            draw_vec(renderer, cell_top_left, cell_bottom_left);
         }
      }

      for (GObject* obj : *statemgr::get_object_list()) {
         auto* collider = obj->get_component("ColliderComponent");
         if (collider != nullptr) {
            CircleCollider const* cc = static_cast<CircleCollider const*>(collider);

            if (cc->is_static()) {
               SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            } else {
               SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            }

            draw_circle(renderer, cc->get_radius(), obj->get_position().x, obj->get_position().y, 3);
         }
      }


      int mx, my;
      SDL_GetMouseState(&mx, &my);
      for (GObject* obj : *statemgr::get_object_list()) {
         auto* collider = obj->get_staging_component("ColliderComponent");
         if (collider != nullptr) {
            CircleCollider* cc = static_cast<CircleCollider*>(collider);
            aabb cursor_area = aabb(vec2(mx - 1, my - 1), vec2(mx + 1, my + 1));

            aabb collider_area = cc->bounding_box();
            collider_area.shift(obj->get_staging_position());

            if (collider_area.overlap(cursor_area)) {
               // run a query on this object
               std::vector<ColliderComponent*> colliders;
               quad_part.query(cc, colliders);
               int static_sz = colliders.size();
               SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
               for (ColliderComponent* cc2 : colliders) {
                  aabb cc2_area = cc2->bounding_box();
                  cc2_area.shift(cc2->get_parent()->get_staging_position());

                  draw_box(renderer, cc2_area);
               }

               colliders.clear();
               
               cell_part.query(cc, colliders);
               SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
               for (ColliderComponent* cc2 : colliders) {
                  aabb cc2_area = cc2->bounding_box();
                  cc2_area.shift(cc2->get_parent()->get_staging_position());

                  draw_box(renderer, cc2_area);
               }
               printf("Num static queried: %d, dynamic: %d\n", static_sz, colliders.size());
            }
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
   GhettoExtension ext;

   srand(time(NULL));
   for (int i = 0; i < 100; i++) {
      statemgr::reset_game();
      GObject* n = new GObject;
      statemgr::get_object_list()->add_object(n);
      float rx = rand() % 980 + 10;
      float ry = rand() % 980 + 10;

      n->init(vec2(rx, ry), 0, vec2(1, 1), std::to_string(i), false, false);
      if (i < 40) {
         n->add_component(std::make_unique<CircleCollider>(false, vec2(0, 0), vec2(), 30));
      } else {
         n->add_component(std::make_unique<CircleCollider>(true, vec2(20, 20), vec2(20, 20), 2));
      }
   }

   statemgr::add_extension(new GhettoExtension());
   statemgr::core_game_loop(1.f / 60.f);
}