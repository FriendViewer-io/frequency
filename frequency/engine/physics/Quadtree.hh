#pragma once

#include <array>
#include <vector>

#include "engine/math/AABB.hh"
#include "engine/physics/SpatialPartition.hh"

class ColliderComponent;

class Quadtree : public SpatialPartition {
public:
   Quadtree(aabb const& bounds, int level) : cell_bounds(bounds), level(level) {}

   void insert(aabb const& bounds, ColliderComponent* cc) override;
   void query(aabb const& bounds, std::vector<ColliderComponent*>& results_out) override;
   void remove(aabb const& bounds, ColliderComponent* cc) override;
   void clear();

   int get_level() const { return level; }

   template <typename T>
   void for_each_level(T&& cb) {
      cb(cell_bounds);
      if (!leaf) {
         for (Quadtree* qt : children) {
            qt->for_each_level(cb);
         }
      }
   }

   virtual ~Quadtree() { clear(); }

private:
   void collect(std::vector<ColliderComponent*>& out);
   void subdivide();

   static constexpr int MAX_COLLIDERS = 2;
   static constexpr int MAX_LEVELS = 5;

   aabb cell_bounds;
   int level;
   bool leaf = true;

   std::array<Quadtree*, 4> children;

   std::vector<ColliderComponent*> contained_objects;
};