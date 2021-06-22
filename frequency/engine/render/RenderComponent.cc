#include "engine/render/RenderComponent.hh"

#include <SDL_opengl.h>

#include "engine/render/RenderMeshResource.hh"
#include "engine/render/ShaderResource.hh"

void RenderComponent::bind_data() const {
   if (shader) {
      shader->use_program();
   }
   if (mesh) {
      mesh->bind_mesh();
   }
}

int RenderComponent::get_draw_count() const { return mesh->get_index_count(); }
