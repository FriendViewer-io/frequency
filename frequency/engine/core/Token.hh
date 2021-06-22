#pragma once

#include <memory>

#include "engine/core/EngineResource.hh"

namespace detail {
void free_resource(ResourceRef* res);
}

template <typename T>
class Token {
public:
   Token() { resource = nullptr; }
   Token(Token<T> const& rv) : Token(rv.resource) {}
   Token(Token<T>&& rv) : resource(rv.resource) {
      rv.resource = nullptr;
   }

   Token<T>& operator=(Token<T> const& rv) {
      resource = rv.resource;
      resource->ref_count++;
      return *this;
   }
   Token<T>& operator=(Token<T>&& rv) {
      resource = rv.resource;
      rv.resource = nullptr;
      return *this;
   }

   operator bool() const { return resource != nullptr; }
   T const* get_inner() const { return static_cast<T const*>(resource->inner_data.get()); }
   T const& operator*() const { return *get_inner(); }
   T const* operator->() const { return get_inner(); }
   ~Token() {
      if (resource != nullptr) {
         resource->ref_count--;
         if (resource->ref_count <= 0) {
            detail::free_resource(resource);
         }
         resource = nullptr;
      }
   }

private:
   Token(ResourceRef* resource) : resource(resource) { resource->ref_count++; }

   mutable ResourceRef* resource;
   friend class ResourceManager;
};