#pragma once

#include <vector>

#include "engine/core/GObject.hh"

class GOList {
public:
   void add_object(GObject* obj);
   void remove_object(GObject* obj);
   void remove_object(std::string const& name);
   void remove_object(int id);

   GObject* get_object(std::string const& name);
   GObject* get_object(int id);

   template <typename T>
   void for_each(T&& fn) {
      for (GObject* go : object_list) {
         fn(go);
      }
   }

private:
   std::vector<GObject*> object_list;
};