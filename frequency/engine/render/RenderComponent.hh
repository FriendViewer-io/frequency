#pragma once

#include "engine/core/Component.hh"
#include "engine/core/Token.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"

class RenderMeshResource;
class ShaderResource;

class RenderComponent : public Component {
public:
   RenderComponent() {}

   void on_tick(float dt) override {}
   void on_post_tick(float dt) const override {}
   void on_message(GObject* sender, std::string const& msg) override {}

   uint32_t get_component_flags() const { return Component::NO_CLONE_FLAG; }

   std::string_view get_component_type_name() const final { return "RenderComponent"; }
   
   virtual void bind_data() const;
   int get_draw_count() const;

   virtual ~RenderComponent() {}

protected:
   Token<RenderMeshResource> mesh;
   Token<ShaderResource> shader;
};