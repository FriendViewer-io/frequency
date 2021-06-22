#pragma once

#include <vector>

#include "engine/core/EngineResource.hh"

struct MeshDescriptor {
   std::vector<float> raw_vertex_data;
   std::vector<uint32_t> raw_index_data;
   std::vector<int> vertex_attribute_offsets;
};

class RenderMeshResource : public Resource {
public:
   RenderMeshResource();
   void load_mesh(MeshDescriptor& mesh_data);
   void bind_mesh() const;
   int get_index_count() const { return index_count; }

   virtual ~RenderMeshResource();

private:
   uint32_t VAO;
   uint32_t VBO;
   uint32_t EBO;
   int index_count;
   bool loaded;
};
