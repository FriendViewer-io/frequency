#pragma once

#include "engine/core/Component.hh"
#include "engine/core/ComponentGen.hh"
#include "engine/core/Token.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"

class RenderMeshResource;
class ShaderResource;
class Camera;

class RenderComponent : public Component {
   DEFINE_COMPONENT_INLINE_NODEP(RenderComponent, Component)

public:
   void on_tick(float dt) override {}
   void on_post_tick(float dt) const override {}
   void on_message(GObject* sender, std::string const& msg) override {}

   uint32_t get_component_flags() const { return Component::NO_CLONE_FLAG; }

   virtual void bind_data(Camera const* camera) const;
   int get_draw_count() const;
   virtual aabb const& get_render_bounds() const = 0;

   virtual ~RenderComponent() {}

protected:
   Token<RenderMeshResource> mesh;
   Token<ShaderResource> shader;
};