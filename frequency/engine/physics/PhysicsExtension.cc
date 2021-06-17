#include "engine/physics/PhysicsExtension.hh"

#include "engine/core/GOList.hh"
#include "engine/core/StateManager.hh"
#include "engine/physics/CellPartition.hh"
#include "engine/physics/ColliderComponent.hh"
#include "engine/physics/CollisionHelper.hh"
#include "engine/physics/Quadtree.hh"

PhysicsExtension::PhysicsExtension(aabb world_bounds, vec2 cell_dims) {
   quadtree = new Quadtree(world_bounds, 0);
   cell_partition = new CellPartition(world_bounds, cell_dims);
}

// Collect all GObjects with a ColliderComponent
// and add them to collider_objects
void PhysicsExtension::pre_tick(float dt) {
   GOList* object_list = statemgr::get_object_list();

   for (GObject* go : *object_list) {
      if (go->get_component("ColliderComponent")) {
         collider_objects.push_back(go);
      }
   }
}

void PhysicsExtension::pre_post_tick(float dt) {
   std::vector<ColliderComponent*> possible_collisions;
   for (int i = 0; i < collider_objects.size(); i++) {
      GObject* lhs = collider_objects[i];
      ColliderComponent* cc1 =
          static_cast<ColliderComponent*>(lhs->get_staging_component("ColliderComponent"));
      if (cc1->is_static()) {
         continue;
      }
      aabb cc1_bbox = cc1->bounding_box();
      cc1_bbox.shift(lhs->get_position());

      possible_collisions.clear();
      quadtree->query(cc1_bbox, possible_collisions);
      cell_partition->query(cc1_bbox, possible_collisions);
      for (int j = 0; j < possible_collisions.size(); j++) {
         ColliderComponent* cc2 = possible_collisions[j];
         if (cc2 == cc1) {
            continue;
         }

         collision::ContactManifold manifold;
         if (collision::overlap_test(cc1, cc2, manifold)) {

         }
      }
   }
}