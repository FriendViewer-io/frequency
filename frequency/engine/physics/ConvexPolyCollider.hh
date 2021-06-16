#pragma once

#include "engine/physics/ColliderComponent.hh"

class ConvexPolyCollider : public ColliderComponent {
public:
   ConvexPolyCollider(bool is_static, vec2 start_vel, vec2 start_accel)
       : ColliderComponent(is_static, start_vel, start_accel) {}

   virtual int get_num_vertices() const = 0;
   virtual vec2 get_vertex(int index) const = 0;

   vec2 support(vec2 d) const;
   vec2 get_vertex_absolute(int index) const;

   virtual ~ConvexPolyCollider() {}
};