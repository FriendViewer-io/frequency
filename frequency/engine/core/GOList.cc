#include "engine/core/GOList.hh"

void GOList::add_object(GObject* obj) { object_list.push_back(obj); }

void GOList::remove_object(GObject* obj) {
   for (int i = 0; i < object_list.size(); i++) {
      if (object_list[i] == obj) {
         object_list.erase(object_list.begin() + i);
         break;
      }
   }
}

void GOList::remove_object(std::string const& name) {
   for (int i = 0; i < object_list.size(); i++) {
      if (object_list[i]->get_name() == name) {
         object_list.erase(object_list.begin() + i);
         break;
      }
   }
}

void GOList::remove_object(int id) {
   for (int i = 0; i < object_list.size(); i++) {
      if (object_list[i]->get_id() == id) {
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