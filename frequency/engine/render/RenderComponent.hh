#pragma once

#include "engine/core/Component.hh"
#include "engine/math/AABB.hh"
#include "engine/math/Vector.hh"

enum class RenderShape { CIRCLE, POLY };

class RenderShapeComponent : public Component {
public:
   RenderShapeComponent() {}

   void on_tick(float dt) override;
   void on_post_tick(float dt) override {}
   void on_message(GObject* sender, std::string const& msg) override {}

   std::string_view get_component_type_name() const final { return "RenderShapeComponent"; }
   void commit(Component const& from) override;

   virtual RenderShape get_shape() const = 0;

   virtual ~RenderShapeComponent();

protected:

};