#pragma once

#include <vector>

#include "engine/core/Extension.hh"

class GObject;

class PhysicsExtension : public Extension {
public:
   void extension_init() override {}
   void pre_tick(float dt) override;
   // Detect [& solve collisions]
   void pre_post_tick(float dt) override;
   void tick_end(float dt) override { collider_objects.clear(); }
   void extension_reset() override {}

   std::string_view extension_name() const { return "PhysicsExtension"; }

private:
   std::vector<GObject*> collider_objects;
};