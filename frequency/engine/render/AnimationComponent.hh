#pragma once

#include <string_view>

#include "engine/core/Token.hh"
#include "engine/render/RenderComponent.hh"

class SpritesheetResource;
struct AnimData;

class AnimationComponent : public RenderComponent {
public:
   AnimationComponent();

   void on_tick(float dt) override;
   void on_post_tick(float dt) const override;
   void set_animation(std::string_view name);

   void load_data(std::string_view sprite_data_source);
   void bind_data(Camera const* camera) const override;
   vec2 const& get_offset() const { return center_offset; }
   void set_offset(vec2 const& offset) { center_offset = offset; }

   aabb const& get_render_bounds() const override { return render_bounds; }

   virtual ~AnimationComponent();

private:
   Token<SpritesheetResource> sheet_data;
   vec2 center_offset;
   vec2 anchor_offset;
   AnimData const* cur_animation;
   int cur_frame;
   int ticks_on_frame;
   mutable aabb render_bounds;
};