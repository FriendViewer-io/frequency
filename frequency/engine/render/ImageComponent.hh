#pragma once

#include <string_view>

#include "engine/core/ComponentGen.hh"
#include "engine/core/Token.hh"
#include "engine/render/RenderComponent.hh"

class ImageResource;

class ImageComponent : public RenderComponent {
   DEFINE_COMPONENT_INLINE_NODEP(ImageComponent, RenderComponent)

public:
   void on_post_tick(float dt) const override;

   void load_data(std::string_view image_source);
   void bind_data(Camera const* camera) const override;
   vec2 const& get_offset() const { return center_offset; }
   void set_offset(vec2 const& offset) { center_offset = offset; }
   vec2 const& get_anchor() const { return anchor_offset; }
   void set_anchor(vec2 const& anchor) { anchor_offset = anchor; }
   float get_base_width() const;
   float get_base_height() const;
   float get_scaled_width() const;
   float get_scaled_height() const;

   aabb const& get_render_bounds() const override { return render_bounds; }

   virtual ~ImageComponent();

private:
   Token<ImageResource> tex_data;
   vec2 center_offset;
   vec2 anchor_offset;
   mutable aabb render_bounds;
};