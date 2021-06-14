#pragma once

#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/SpatialPartition.hh"

#include <vector>

class ColliderComponent;

class CellPartition : public SpatialPartition {
public:
   CellPartition(aabb const& world_bounds, vec2 cell_dimensions);

   void insert(ColliderComponent* cc) override;
   void query(ColliderComponent* cc, std::vector<ColliderComponent*>& query_result) override;
   void remove(ColliderComponent* cc) override;
   void clear() override;

   virtual ~CellPartition() {}

private:
   aabb world_bounds;
   vec2 cell_dimensions;

   std::vector<std::vector<std::vector<ColliderComponent*>>> object_map;
};