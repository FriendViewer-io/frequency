#pragma once

#include <string>

class GObject;

class Component {
public:
   virtual void on_tick(float dt) = 0;
   virtual void on_post_tick(float dt) = 0;
   virtual void on_message(GObject* sender, std::string const& msg) = 0;
   virtual void commit(Component const& from) = 0;

   virtual ~Component() {}

protected:
   GObject* parent;

private:

};