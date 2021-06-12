#include "engine/physics/PhysicsExtension.hh"

#include "engine/core/GOList.hh"
#include "engine/core/StateManager.hh"
#include "engine/physics/ColliderComponent.hh"
#include "engine/physics/CollisionHelper.hh"

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
   for (int i = 0; i < collider_objects.size(); i++) {
      GObject* lhs = collider_objects[i];
      ColliderComponent* cc1 = static_cast<ColliderComponent*>(lhs->get_staging_component("ColliderComponent"));

      for (int j = i + 1; j < collider_objects.size(); j++) {
         GObject *rhs = collider_objects[j];
         ColliderComponent* cc2 = static_cast<ColliderComponent*>(rhs->get_staging_component("ColliderComponent"));

         if (collision::overlap_test_gjk(cc1, cc2)) {
            printf("Overlap spotted!\n");
         } else {
            printf("No overlap happening...\n");
         }
      }
   }
}