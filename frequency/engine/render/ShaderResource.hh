#pragma once

#include <map>
#include <memory>
#include <string_view>

#include "engine/core/EngineResource.hh"
#include "engine/math/Vector.hh"

class ShaderResource : public Resource {
public:
   void load_shader(std::string_view vs_data, std::string_view fs_data);
   void load_all_uniforms();
   void use_program() const;
   void set_uniform(std::string_view name, float val) const;
   void set_uniform(std::string_view name, vec2 val) const;
   void set_uniform_i(std::string_view name, int val) const;

   ~ShaderResource();

private:
   unsigned int shader_program;
   std::map<std::string, int, std::less<>> uniform_map;
};

std::unique_ptr<Resource> load_shader_from_file(std::string_view path);