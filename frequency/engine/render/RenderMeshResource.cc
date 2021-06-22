#include "engine/render/RenderMeshResource.hh"

#include <gl/glew.h>

RenderMeshResource::RenderMeshResource() : loaded(false), index_count(0) {}

void RenderMeshResource::load_mesh(MeshDescriptor& mesh_data) {
   if (loaded) {
      return;
   }
   glGenVertexArrays(1, &VAO);
   glGenBuffers(1, &VBO);
   glGenBuffers(1, &EBO);

   glBindVertexArray(VAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, mesh_data.raw_vertex_data.size() * sizeof(float),
                mesh_data.raw_vertex_data.data(), GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.raw_index_data.size() * sizeof(uint32_t),
                mesh_data.raw_index_data.data(), GL_STATIC_DRAW);

   int total_offset = 0;
   for (int i : mesh_data.vertex_attribute_offsets) {
      total_offset += i;
   }

   int accum_offset = 0;
   for (int i = 0; i < mesh_data.vertex_attribute_offsets.size(); i++) {
      glVertexAttribPointer(i, mesh_data.vertex_attribute_offsets[i], GL_FLOAT, GL_FALSE,
                            total_offset * sizeof(float),
                            reinterpret_cast<void*>(accum_offset * sizeof(float)));

      glEnableVertexAttribArray(i);
      accum_offset += mesh_data.vertex_attribute_offsets[0];
   }

   loaded = true;
   index_count = static_cast<int>(mesh_data.raw_index_data.size());
}

void RenderMeshResource::bind_mesh() const {
   if (!loaded) {
      return;
   }
   glBindVertexArray(VAO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}

RenderMeshResource::~RenderMeshResource() {
   glDeleteVertexArrays(1, &VAO);
   glDeleteBuffers(1, &VBO);
   glDeleteBuffers(1, &EBO);

   loaded = false;
   VAO = VBO = EBO = 0;
}