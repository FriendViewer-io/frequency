#include "engine/core/Token.hh"

#include "engine/core/ResourceManager.hh"

namespace detail {
void free_resource(ResourceRef* res) { ResourceManager::free_resource(res->res_name); }
}  // namespace detail