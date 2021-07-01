#include "engine/physics/PhysicsExtension.hh"

#include "engine/core/GOList.hh"
#include "engine/core/StateManager.hh"
#include "engine/physics/CellPartition.hh"
#include "engine/physics/ColliderComponent.hh"
#include "engine/physics/CollisionHelper.hh"
#include "engine/physics/Quadtree.hh"
#include "engine/physics/Solver.hh"

PhysicsExtension::PhysicsExtension(aabb world_bounds, vec2 cell_dims) : gravity(0, -100.f) {
   quadtree = new Quadtree(world_bounds, 0);
   cell_partition = new CellPartition(world_bounds, cell_dims);
}

void PhysicsExtension::pre_post_tick(float dt) {
   std::vector<ColliderComponent*> possible_collisions;
   for (int i = 0; i < colliders.size(); i++) {
      ColliderComponent* cc1 = colliders[i];
      if (cc1->is_static()) {
         continue;
      }
      aabb cc1_bbox = cc1->bounding_box();
      cc1_bbox.shift(cc1->position());

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
            solver::solve_collision(manifold);
            cc1->get_parent()->commit();
            cc2->get_parent()->commit();
         }
      }
   }
}

void PhysicsExtension::object_added(GObject* obj) {
   ColliderComponent* cc = obj->get_component<ColliderComponent>();
   if (cc != nullptr) {
      colliders.push_back(cc);
   }
}

void PhysicsExtension::object_removed(GObject* obj) {
   for (int i = 0; i < colliders.size(); i++) {
      if (obj == colliders[i]->get_parent()) {
         colliders.erase(colliders.begin() + i);
         break;
      }
   }
}
