#pragma once

#include <fstream>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>

#include "engine/core/EngineResource.hh"
#include "engine/core/Token.hh"

class ResourceManager {
public:
   using ProcessCB = std::function<std::unique_ptr<Resource>(std::ifstream&)>;

   template <typename T>
   static std::optional<Token<T>> load_file(std::string_view path, ProcessCB&& loader) {
      auto it = active_resources.find(path);
      if (it == active_resources.end()) {
         std::ifstream res_file(path);
         if (res_file.is_open()) {
            std::unique_ptr<Resource> processed = std::move(loader(res_file));
            auto map_entry = active_resources.insert(
                std::make_pair(std::string(path), ResourceRef(std::move(processed))));
            map_entry.first->second.res_name = map_entry.first->first;
            return Token<T>(&map_entry.first->second);
         } else {
            return std::nullopt;
         }
      }
      return Token<T>(&it->second);
   }

   template <typename T>
   static std::optional<Token<T>> wrap_resource(std::string_view name,
                                                std::unique_ptr<Resource> r) {
      auto it = active_resources.find(name);
      if (it == active_resources.end()) {
         auto map_entry =
             active_resources.insert(std::make_pair(std::string(name), ResourceRef(std::move(r))));
         map_entry.first->second.res_name = map_entry.first->first;
         return Token<T>(&map_entry.first->second);
      }
      return std::nullopt;
   }

   template <typename T>
   static std::optional<Token<T>> lookup_resource(std::string_view name) {
      auto it = active_resources.find(name);
      if (it == active_resources.end()) {
         return std::nullopt;
      }
      return Token<T>(&it->second);
   }

   static bool resource_loaded(std::string_view name) {
      auto it = active_resources.find(name);
      return it != active_resources.end();
   }

   static void free_resource(std::string_view name) {
      auto it = active_resources.find(name);
      if (it != active_resources.end()) {
         it->second.inner_data.reset();
         active_resources.erase(it);
      }
   }

private:
   inline static std::map<std::string, ResourceRef, std::less<>> active_resources;
};