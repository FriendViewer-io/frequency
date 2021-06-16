#pragma once

#include <vector>

#include "engine/math/AABB.hh"

class ColliderComponent;

class SpatialPartition {
public:
   virtual void insert(aabb const& bounds, ColliderComponent* cc) = 0;
   virtual void query(aabb const& bounds, std::vector<ColliderComponent*>& query_result) = 0;
   virtual void remove(aabb const& bounds, ColliderComponent* cc) = 0;
   virtual void clear() = 0;

   virtual ~SpatialPartition() {}

private:
};