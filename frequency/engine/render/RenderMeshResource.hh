#pragma once

#include <vector>

#include "engine/core/EngineResource.hh"

struct MeshDescriptor {
   std::vector<float> raw_vertex_data;
   std::vector<int> raw_index_data;
   std::vector<int> vertex_attribute_offsets;
};

class RenderMeshResource : public Resource {
public:
   RenderMeshResource();
   void load_mesh(MeshDescriptor& mesh_data);
   void bind_mesh();

   virtual ~RenderMeshResource();

private:
   uint32_t VAO;
   uint32_t VBO;
   uint32_t EBO;
   bool loaded;
};
