#pragma once

#include <memory>

#include "engine/core/EngineResource.hh"

namespace detail {
void free_resource(ResourceRef* res);
}

template <typename T>
class Token {
public:
   Token() = delete;
   Token(Token<T> const& rv) : Token(rv.resource) {}
   Token(Token<T>&& rv) : resource(rv.resource) {
      rv.resource = nullptr;
   }

   T const* get_inner() const { return static_cast<T const*>(resource->inner_data); }
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
   Token(ResourceRef* resource) : resource(resource) { resource.ref_count++; }

   ResourceRef* resource;
   friend class ResourceManager;
};