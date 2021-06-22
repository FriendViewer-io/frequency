
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

int main() {
   statemgr::core_game_loop(1.f / 60.f, [] {
      GObject* sq1 = new GObject;
      sq1->init(vec2(1000, 500), 0, vec2(1, 1), "sq1", false, false);
      auto image_comp = sq1->create_component<ImageComponent>();
      image_comp->load_data("test/images/patchy.png");
      image_comp->set_anchor(
          vec2(image_comp->get_scaled_width() / -2, image_comp->get_scaled_height() / 2));
      statemgr::get_object_list()->add_object(sq1);
   });
}
