#include "engine/render/RenderComponent.hh"

#include "engine/render/RenderMeshResource.hh"
#include "engine/render/ShaderResource.hh"

// clang-format off
#include <SDL_opengl.h>
// clang-format on

void RenderComponent::bind_data(Camera const* camera) const {
   if (shader) {
      shader->use_program();
   }
   if (mesh) {
      mesh->bind_mesh();
   }
}

int RenderComponent::get_draw_count() const { return mesh->get_index_count(); }
