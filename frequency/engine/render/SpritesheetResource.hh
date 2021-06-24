#pragma once

#include <map>
#include <string>
#include <vector>

#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"
#include "engine/render/ImageResource.hh"

struct AnimFrame {
   AnimFrame(int sprite_clip_index, int duration_ticks)
       : sprite_clip_index(sprite_clip_index), duration_ticks(duration_ticks) {}

   int sprite_clip_index;
   int duration_ticks;
};

struct AnimData {
   std::vector<aabb> sprite_clips;
   std::vector<AnimFrame> frames;

   vec2 max_dims;
   std::string anim_name;
};

class SpritesheetResource : public ImageResource {
public:
   SpritesheetResource();

   void load_sheet(std::string_view sheet_path);
   AnimData const* get_animation(std::string_view name) const;

protected:
   std::map<std::string, AnimData, std::less<>> animations;
   bool sheet_loaded;
};