#pragma once

#include <vector>

#include "engine/physics/ConvexPolyCollider.hh"

class MeshCollider : public ConvexPolyCollider {
public:
   MeshCollider(std::vector<vec2>&& vertices, bool s)
       : ConvexPolyCollider(s), vertices(std::move(vertices)) {}

   int get_num_vertices() const override { return vertices.size(); }

   vec2 get_vertex(int index) const override { return vertices[index]; }

   void commit(Component const& from) override {
      ConvexPolyCollider::commit(from);

      MeshCollider const& rhs = static_cast<MeshCollider const&>(from);
      if (vertices.size() != rhs.vertices.size()) {
         vertices = rhs.vertices;
      }
   }

   std::unique_ptr<Component> clone() {
      std::vector<vec2> vtx_copy = vertices;
      return std::make_unique<MeshCollider>(std::move(vtx_copy), is_static());
   }

   void update_bounds() override {
      vec2 mins = vec2(FLT_MAX, FLT_MAX), maxes = vec2(FLT_MIN, FLT_MIN);
      for (vec2 const& v : vertices) {
         mins.x = std::min(v.x, mins.x);
         mins.y = std::min(v.y, mins.y);
         maxes.x = std::max(v.x, maxes.x);
         maxes.y = std::max(v.y, maxes.y);
      }
      precomputed_bounds = aabb(mins, maxes);
   }

   CollisionShape get_shape() const { return CollisionShape::VMESH_CVP; }

private:
   std::vector<vec2> vertices;
};