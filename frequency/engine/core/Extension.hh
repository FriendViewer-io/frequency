#pragma once

#include <string_view>

class GObject;

class Extension {
public:
   // One-time startup initialization
   virtual void extension_init() = 0;
   // Called before any GObject has ticked
   virtual void pre_tick(float dt) = 0;
   // Called after every GObject has ticked, and before any post-tick
   virtual void pre_post_tick(float dt) = 0;
   // Called after every GObject has post-ticked, before the sleep period
   virtual void tick_end(float dt) = 0;
   // Reset to an equivalent startup state
   virtual void extension_reset() = 0;

   virtual void object_added(GObject* obj) = 0;
   virtual void object_removed(GObject* obj) = 0;

   virtual std::string_view extension_name() const = 0;
};