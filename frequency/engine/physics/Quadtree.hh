#pragma once

#include "engine/math/AABB.hh"

#include <array>
#include <vector>

class ColliderComponent;

class Quadtree {
public:
   Quadtree(aabb const& bounds, int level) : cell_bounds(bounds), level(level) {}

   void insert(ColliderComponent* cc);
   void query(ColliderComponent* cc, std::vector<ColliderComponent*>& results_out);

   int get_level() const { return level; }

private:
   void collect(std::vector<ColliderComponent*>& out);
   void subdivide();

   static constexpr int MAX_COLLIDERS = 10;
   static constexpr int MAX_LEVELS = 5;

   aabb cell_bounds;
   int level;
   bool leaf = true;

   std::array<Quadtree*, 4> children;

   std::vector<ColliderComponent*> contained_objects;
};