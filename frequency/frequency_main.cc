
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
      vec2 move;
      if (GetAsyncKeyState(VK_RIGHT)) {
         move.x += 10;
      }
      if (GetAsyncKeyState(VK_LEFT)) {
         move.x -= 10;
      }
      if (GetAsyncKeyState(VK_UP)) {
         move.y += 10;
      }
      ColliderComponent* cc = static_cast<ColliderComponent*>(get_parent()->get_staging_component("ColliderComponent"));
      cc->set_velocity(cc->get_velocity() + move);
   }

   void on_post_tick(float dt) const override {}
   void on_message(GObject* sender, std::string const& msg) override {}
   std::string_view get_component_type_name() const override { return "MoveRight"; }
   uint32_t get_component_flags() const override { return NO_CLONE_FLAG; }

private:
};

int main() {
   statemgr::core_game_loop(1.f / 60.f, [] {
      GObject* sq1 = new GObject;
      sq1->init(vec2(0, 0), 0, vec2(1, 1), "sq1", false, false);
      // auto anim_comp1 = sq1->create_component<AnimationComponent>();
      // anim_comp1->load_data("test/images/testsheet.ss");


      auto anim_comp1 = sq1->create_component<ImageComponent>();
      anim_comp1->load_data("test/images/may.png");

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
