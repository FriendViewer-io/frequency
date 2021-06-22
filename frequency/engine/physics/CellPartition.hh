#pragma once

#include <vector>

#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/SpatialPartition.hh"

class ColliderComponent;

class CellPartition : public SpatialPartition {
public:
   CellPartition(aabb const& world_bounds, vec2 cell_dimensions);

   void insert(aabb const& bounds, ColliderComponent* cc) override;
   void query(aabb const& bounds, std::vector<ColliderComponent*>& query_result) override;
   void remove(aabb const& bounds, ColliderComponent* cc) override;
   void clear() override;

   aabb const& get_world_bounds() const { return world_bounds; }
   vec2 const& get_cell_dimensions() const { return cell_dimensions; }

   virtual ~CellPartition() {}

private:
   aabb world_bounds;
   vec2 cell_dimensions;
   int row_count, col_count;

   std::vector<std::vector<std::vector<ColliderComponent*>>> object_map;
};