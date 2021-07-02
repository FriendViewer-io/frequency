
#include <SDL.h>
#include <conio.h>

#include <thread>

#undef main

#include "engine/core/Component.hh"
#include "engine/core/Extension.hh"
#include "engine/core/GOList.hh"
#include "engine/core/GObject.hh"
#include "engine/core/ResourceManager.hh"
#include "engine/core/StateManager.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/AABoxCollider.hh"
#include "engine/physics/CellPartition.hh"
#include "engine/physics/CircleCollider.hh"
#include "engine/physics/ColliderComponent.hh"
#include "engine/physics/CollisionHelper.hh"
#include "engine/physics/ConvexPolyCollider.hh"
#include "engine/physics/MeshCollider.hh"
#include "engine/physics/PhysicsExtension.hh"
#include "engine/physics/Quadtree.hh"
#include "engine/render/ImageComponent.hh"
#include "engine/render/ImageResource.hh"
#include "engine/render/RenderComponent.hh"
#include "engine/render/RenderExtension.hh"
#include "engine/render/RenderMeshResource.hh"
#include "engine/render/ShaderResource.hh"
#include "engine/render/Camera.hh"
#include "engine/render/SpritesheetResource.hh"
#include "engine/render/AnimationComponent.hh"
#include "engine/core/ComponentGen.hh"
#include "engine/render/TransitionRule.hh"
#include "engine/render/AnimationController.hh"
#include "engine/input/InputExtension.hh"

#include <Windows.h>

class Move : public Component {
   DEFINE_COMPONENT_CLASS_CHUNK(Move, Component, AnimationComponent, ColliderComponent)

public:
   void on_tick(float dt) override;

   void on_post_tick(float dt) const override {}
   void on_message(GObject* sender, std::string const& msg) override {}
   uint32_t get_component_flags() const override { return NO_CLONE_FLAG; }

private:
   AnimData const* last_anim = nullptr;
};

DEFINE_COMPONENT_GLOBAL_CHUNK(Move)

void Move::on_tick(float dt) {
   AnimationComponent* ac = get<AnimationComponent>(this);
   ColliderComponent* cc = get<ColliderComponent>(this);

   float friction = 1.f;
   auto cur_anim = ac->get_cur_animation();
   vec2 move;
   if (GetAsyncKeyState(VK_RIGHT)) {
      move.x += 2000;
      friction = 0;
   }
   if (GetAsyncKeyState(VK_LEFT)) {
      move.x -= 2000;
      friction = 0;
   }
   if (GetAsyncKeyState(VK_UP)) {
      move.y += 2000;
      friction = 0;
   }
   if (GetAsyncKeyState(VK_DOWN)) {
      move.y -= 2000;
      friction = 0;
   }

   vec2 vel = cc->get_velocity() + move * dt;
   vel -= vel * friction;
   if (vel.mag_sq() > 300.f * 300.f) {
      cc->set_velocity(vel.normalized() * 300.f);
   } else {
      cc->set_velocity(vel);
   }
}

struct KeypressTransition : public TransitionRule {
   KeypressTransition(int target_idx, int vk) : TransitionRule(target_idx), vk(vk) {}
   bool passes() override {
      return GetAsyncKeyState(vk) ? true : false;
   }

   const int vk;
};

struct KeyreleaseTransition : public TransitionRule {
   KeyreleaseTransition(int target_idx, int vk) : TransitionRule(target_idx), vk(vk) {}
   bool passes() override {
      return GetAsyncKeyState(vk) ? false : true;
   }

   const int vk;
};


int main() {
   SDL_Init(SDL_INIT_EVERYTHING);

   SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500, 0);

   InputExtension ext;

   while (true) {
      ext.pre_tick(0);


      if (ext.get_key_state(SDL_SCANCODE_RIGHT) == KeyState::RELEASE) {
         printf("<RIGHT> is released.\n");
      }
      if (ext.get_key_state(SDL_SCANCODE_RIGHT) == KeyState::PRESS) {
         printf("<RIGHT> is pressed.\n");
      }
   }

   /*
   statemgr::core_game_loop(1.f / 60.f, [] {
      GObject* sq1 = new GObject;
      sq1->init(vec2(0, 0), 0, vec2(5, 5), "sq1", false, false);
      // auto anim_comp1 = sq1->create_component<AnimationComponent>();
      // anim_comp1->load_data("test/images/testsheet.ss");


      auto anim_comp1 = sq1->create_component<AnimationComponent>();
      anim_comp1->load_data("test/images/testsheet.ss");
      sq1->create_component<Move>();
      
      auto anim_controller_comp1 = sq1->create_component<AnimationController>();
      int wl_state = anim_controller_comp1->add_state("walkleft");
      int wr_state = anim_controller_comp1->add_state("walkright");
      int sl_state = anim_controller_comp1->add_state("standleft");
      int sr_state = anim_controller_comp1->add_state("standright");

      int wu_state = anim_controller_comp1->add_state("walkup");
      int wd_state = anim_controller_comp1->add_state("walkdown");
      int su_state = anim_controller_comp1->add_state("standup");
      int sd_state = anim_controller_comp1->add_state("standdown");

      anim_controller_comp1->add_rule(wl_state, std::make_unique<KeyreleaseTransition>(sl_state, VK_LEFT));
      anim_controller_comp1->add_rule(wr_state, std::make_unique<KeyreleaseTransition>(sr_state, VK_RIGHT));
      anim_controller_comp1->add_rule(wu_state, std::make_unique<KeyreleaseTransition>(su_state, VK_UP));
      anim_controller_comp1->add_rule(wd_state, std::make_unique<KeyreleaseTransition>(sd_state, VK_DOWN));


      anim_controller_comp1->add_rule(sl_state, std::make_unique<KeypressTransition>(wl_state, VK_LEFT));
      anim_controller_comp1->add_rule(sl_state, std::make_unique<KeypressTransition>(wr_state, VK_RIGHT));
      anim_controller_comp1->add_rule(sl_state, std::make_unique<KeypressTransition>(wu_state, VK_UP));
      anim_controller_comp1->add_rule(sl_state, std::make_unique<KeypressTransition>(wd_state, VK_DOWN));

      anim_controller_comp1->add_rule(sr_state, std::make_unique<KeypressTransition>(wl_state, VK_LEFT));
      anim_controller_comp1->add_rule(sr_state, std::make_unique<KeypressTransition>(wr_state, VK_RIGHT));
      anim_controller_comp1->add_rule(sr_state, std::make_unique<KeypressTransition>(wu_state, VK_UP));
      anim_controller_comp1->add_rule(sr_state, std::make_unique<KeypressTransition>(wd_state, VK_DOWN));

      anim_controller_comp1->add_rule(su_state, std::make_unique<KeypressTransition>(wl_state, VK_LEFT));
      anim_controller_comp1->add_rule(su_state, std::make_unique<KeypressTransition>(wr_state, VK_RIGHT));
      anim_controller_comp1->add_rule(su_state, std::make_unique<KeypressTransition>(wu_state, VK_UP));
      anim_controller_comp1->add_rule(su_state, std::make_unique<KeypressTransition>(wd_state, VK_DOWN));

      anim_controller_comp1->add_rule(sd_state, std::make_unique<KeypressTransition>(wl_state, VK_LEFT));
      anim_controller_comp1->add_rule(sd_state, std::make_unique<KeypressTransition>(wr_state, VK_RIGHT));
      anim_controller_comp1->add_rule(sd_state, std::make_unique<KeypressTransition>(wu_state, VK_UP));
      anim_controller_comp1->add_rule(sd_state, std::make_unique<KeypressTransition>(wd_state, VK_DOWN));

      //anim_comp1->set_sort_group(DrawSortGroup::DEFAULT);

      auto coll_comp1 = sq1->create_component<CircleCollider>(false, 50.f);
      coll_comp1->set_gravity_scalar(0.0f);

      // vec2 image_dims = vec2(image_comp1->get_scaled_width(), image_comp1->get_scaled_height());
      // auto coll_comp1 = sq1->create_component<AABoxCollider>(image_dims * 0.5f, false);
      // coll_comp1->set_gravity_scalar(1);

      GObject* sq2 = new GObject;
      sq2->init(vec2(0, 0), 0, vec2(0.1, 0.1), "sq2", false, false);
      auto image_comp2 = sq2->create_component<ImageComponent>();
      image_comp2->load_data("test/images/patchy.png");
      image_comp2->set_sort_group(DrawSortGroup::BACKGROUND);
      // image_dims = vec2(image_comp2->get_scaled_width(), image_comp2->get_scaled_height());
      // sq2->create_component<AABoxCollider>(image_dims * 0.5f, true);

      // sq1->create_component<Move>();
      

      // statemgr::get_render_extension()->get_camera()->track_object(sq1);
      
      statemgr::get_object_list()->add_object(sq1);
      statemgr::get_object_list()->add_object(sq2);
   });
   */
}
