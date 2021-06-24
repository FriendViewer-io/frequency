
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

#include <Windows.h>

class Move : public Component {
public:
   void on_tick(float dt) override {
      AnimationComponent* ac = static_cast<AnimationComponent*>(get_parent()->get_staging_component("RenderComponent"));
      auto cur_anim = ac->get_cur_animation();
      vec2 move;
      if (GetAsyncKeyState(VK_RIGHT)) {
         if (cur_anim->anim_name != "walkright") {
            last_anim = cur_anim;
            ac->set_animation("walkright");
         }
         move.x += 300;
      }
      if (GetAsyncKeyState(VK_LEFT)) {
         if (cur_anim->anim_name != "walkleft") {
            last_anim = cur_anim;
            ac->set_animation("walkleft");
         }
         move.x -= 300;
      }
      if (GetAsyncKeyState(VK_UP)) {
         if (cur_anim->anim_name != "walkup") {
            last_anim = cur_anim;
            ac->set_animation("walkup");
         }
         move.y += 300;
      }
      if (GetAsyncKeyState(VK_DOWN)) {
         if (cur_anim->anim_name != "walkdown") {
            last_anim = cur_anim;
            ac->set_animation("walkdown");
         }
         move.y -= 300;
      }
      
      parent_data->position += move * dt;
   }

   void on_post_tick(float dt) const override {}
   void on_message(GObject* sender, std::string const& msg) override {}
   std::string_view get_component_type_name() const override { return "Move"; }
   uint32_t get_component_flags() const override { return NO_CLONE_FLAG; }

private:
   AnimData const* last_anim = nullptr;
};

int main() {
   statemgr::core_game_loop(1.f / 60.f, [] {
      GObject* sq1 = new GObject;
      sq1->init(vec2(0, 0), 0, vec2(5, 5), "sq1", false, false);
      // auto anim_comp1 = sq1->create_component<AnimationComponent>();
      // anim_comp1->load_data("test/images/testsheet.ss");


      auto anim_comp1 = sq1->create_component<AnimationComponent>();
      anim_comp1->load_data("test/images/testsheet.ss");
      anim_comp1->set_animation("walkdown");
      sq1->create_component<Move>();

      // vec2 image_dims = vec2(image_comp1->get_scaled_width(), image_comp1->get_scaled_height());
      // auto coll_comp1 = sq1->create_component<AABoxCollider>(image_dims * 0.5f, false);
      // coll_comp1->set_gravity_scalar(1);

      // GObject* sq2 = new GObject;
      // sq2->init(vec2(0, 0), 0, vec2(10, 0.2), "sq2", false, false);
      // auto image_comp2 = sq2->create_component<ImageComponent>();
      // image_comp2->load_data("test/images/patchy.png");
      // image_dims = vec2(image_comp2->get_scaled_width(), image_comp2->get_scaled_height());
      // sq2->create_component<AABoxCollider>(image_dims * 0.5f, true);

      // sq1->create_component<Move>();
      

      // statemgr::get_render_extension()->get_camera()->track_object(sq1);



      
      statemgr::get_object_list()->add_object(sq1);
      // statemgr::get_object_list()->add_object(sq2);
   });
}
