#pragma once

#include "engine/math/Vector.hh"

class ColliderComponent;
class CircleCollider;
class ConvexPolyCollider;

namespace collision {

struct ContactManifold {
   ColliderComponent* a;
   ColliderComponent* b;

   // deepest penetrating points
   vec2 pa;
   vec2 pb;

   // resolution normal
   vec2 normal;
   float overlap_distance;

   vec2 tangent;
};

bool overlap_test_gjk(ColliderComponent* a, ColliderComponent* b);

bool overlap_test_circle_circle(CircleCollider* a, CircleCollider* b);
bool overlap_test_circle_convex(CircleCollider* a, ConvexPolyCollider* b);
bool overlap_test(ColliderComponent* a, ColliderComponent* b);

void generate_contacts_circle_circle(CircleCollider* a, CircleCollider* b, ContactManifold& manifold);
void generate_contacts(ColliderComponent* a, ColliderComponent* b, ContactManifold& manifold);

}