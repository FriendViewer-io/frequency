#include "engine/physics/PhysicsExtension.hh"

#include "engine/math/Vector.hh"
#include "engine/core/GOList.hh"
#include "engine/core/StateManager.hh"
#include "engine/physics/ColliderComponent.hh"

#include <array>

struct Simplex {
   std::array<vec2, 3> points;
   int size = 0;

   Simplex() : points({0, 0, 0}) {}

   vec2& operator[](unsigned i) { return points[i]; }
   int size() const { return size; }
   auto begin() const { return points.begin(); }
   auto end() const { return points.end() - (3 - size); }

   void push_front(vec2 point) {
      points = {point, points[0], points[1] };
      size = std::min(size + 1, 3);
   }

   Simplex& operator=(std::initializer_list<vec2> list) {
      for (auto v = list.begin(); v != list.end(); v++) {
         points[std::distance(list.begin(), v)] = *v;
      }
      size = list.size();

      return *this;
   }
};

vec2 support(ColliderComponent* a, ColliderComponent* b, vec2 v) {
   return a->support(vec2(1, 0)) - b->support(vec2(-1, 0));
}

inline bool same_direction(vec2 const& d, vec2 const& ao) {
   return d.dot(ao) > 0.f;
}

vec2 triple(vec2 a, vec2 b, vec2 c) {
   return b * c.dot(a) - a * c.dot(b);
}

// https://gist.github.com/shiiion/db604f546813a1f80fbd62cfd2bc5bdb

bool line(Simplex& points, vec2& direction) {
   vec2 ab = points[0] - points[1];

   direction = triple(ab, -points[0], ab);
   return false;
}

bool triangle(Simplex& points, vec2& direction) {
   vec2 a = points[2], b = points[1], c = points[0];
   vec2 ao = -a;
   vec2 ab = b - a;
   vec2 ac = c - a;
   vec2 pab = triple(ac, ab, ab);
   vec2 pac = triple(ab, ac, ac);

   if (same_direction(pab, ao)) {
      points = {points[1], points[2]};
      direction = pab;
   } else if (same_direction(pac, ao)) {
      points = {points[0], points[2]};
      direction = pac;
   } else {
      return true;
   }
   return false;
}

bool next_simplex(Simplex& points, vec2& direction) {
   switch (points.size) {
      case 2: return line(points, direction);
      case 3: return triangle(points, direction);
   }
   return false;
}

bool do_gjk(ColliderComponent* a, ColliderComponent* b) {
   vec2 s = support(a, b, vec2(1, 0));

   Simplex points;
   points.push_front(s);

   vec2 direction = vec2(-s.x, -s.y);

   while (true) {
      s = support(a, b, direction);

      if (s.dot(direction) <= 0.f) {
         return false;
      }

      points.push_front(s);

      if (next_simplex(points, direction)) {
         return true;
      }
   }
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
   for (int i = 0; i < collider_objects.size(); i++) {
      for (int j = i + 1; j < collider_objects.size(); j++) {
         GObject* lhs = collider_objects[i], *rhs = collider_objects[j];
         
         // if (do_gjk(lhs->component_, rhs)) {
            // printf("They collide\n");
         // }
      }
   }
}