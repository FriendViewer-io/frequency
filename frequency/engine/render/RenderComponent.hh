#pragma once

#include "engine/core/Component.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"


class RenderComponent : public Component {
public:
   RenderComponent() {}

   void on_tick(float dt) override {}
   void on_post_tick(float dt) const override {}
   void on_message(GObject* sender, std::string const& msg) override {}

   std::string_view get_component_type_name() const final { return "RenderComponent"; }
   void commit(Component const& from) override;

   void bind_data();

   virtual ~RenderComponent() {}

protected:
   

};