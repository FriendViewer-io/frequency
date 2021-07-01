#include "engine/core/GOList.hh"

#include "engine/core/StateManager.hh"
#include "engine/core/Extension.hh"

void GOList::add_object(GObject* obj) {
   if (obj->added_to_object_list()) {
      object_list.push_back(obj);
      for (Extension* ext : statemgr::get_extension_list()) {
         ext->object_added(obj);
      }
   } else {
      // log failure?
   }
}

void GOList::remove_object(GObject* obj) {
   for (int i = 0; i < object_list.size(); i++) {
      if (object_list[i] == obj) {
         for (Extension* ext : statemgr::get_extension_list()) {
            ext->object_removed(object_list[i]);
         }
         object_list.erase(object_list.begin() + i);
         break;
      }
   }
}

void GOList::remove_object(std::string const& name) {
   for (int i = 0; i < object_list.size(); i++) {
      if (object_list[i]->get_name() == name) {
         for (Extension* ext : statemgr::get_extension_list()) {
            ext->object_removed(object_list[i]);
         }
         object_list.erase(object_list.begin() + i);
         break;
      }
   }
}

void GOList::remove_object(int id) {
   for (int i = 0; i < object_list.size(); i++) {
      if (object_list[i]->get_id() == id) {
         for (Extension* ext : statemgr::get_extension_list()) {
            ext->object_removed(object_list[i]);
         }
         object_list.erase(object_list.begin() + i);
         break;
      }
   }
}

GObject* GOList::get_object(std::string const& name) {
   for (int i = 0; i < object_list.size(); i++) {
      if (object_list[i]->get_name() == name) {
         return object_list[i];
      }
   }
   return nullptr;
}

GObject* GOList::get_object(int id) {
   for (int i = 0; i < object_list.size(); i++) {
      if (object_list[i]->get_id() == id) {
         return object_list[i];
      }
   }
   return nullptr;
}