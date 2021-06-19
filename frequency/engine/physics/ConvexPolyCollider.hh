#pragma once

#include "engine/physics/ColliderComponent.hh"

class ConvexPolyCollider : public ColliderComponent {
public:
   ConvexPolyCollider(bool is_static) : ColliderComponent(is_static) {}

   virtual int get_num_vertices() const = 0;
   virtual vec2 get_vertex(int index) const = 0;

   void update_centroid() override;
   vec2 support(vec2 d) const override;

   virtual ~ConvexPolyCollider() {}
};