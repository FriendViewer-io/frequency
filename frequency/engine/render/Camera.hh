#pragma once

#include "engine/math/Vector.hh"
#include "engine/math/AABB.hh"

class GObject;
class RenderComponent;

class Camera {
public:
   Camera();

   void track_object(GObject const* target);
   void update_tracking();
   vec2 get_adjusted_center(GObject const* target) const;
   void dimensions_changed(vec2 const& new_dims);

   bool should_cull(RenderComponent const* rc);

private:
   // center of screen
   vec2 position;
   // dimensions of screen
   vec2 dimensions;
   // bounds of screen
   aabb bounds;

   GObject const* tracked_object;
};