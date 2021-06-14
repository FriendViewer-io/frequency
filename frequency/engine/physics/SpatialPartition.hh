#pragma once

#include <vector>

class ColliderComponent;

class SpatialPartition {
public:
   virtual void insert(ColliderComponent* cc) = 0;
   virtual void query(ColliderComponent* cc, std::vector<ColliderComponent*>& query_result) = 0;
   virtual void remove(ColliderComponent* cc) = 0;
   virtual void clear() = 0;

   virtual ~SpatialPartition() {}
private:
};