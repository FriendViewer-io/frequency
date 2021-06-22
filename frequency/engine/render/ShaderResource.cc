#include "ShaderResource.hh"

#include <array>
#include <fstream>
#include <sstream>

// clang-format off
#include <gl/glew.h>
#include <SDL_opengl.h>
// clang-format on

std::unique_ptr<Resource> load_shader_from_file(std::string_view path) {
   std::ifstream fs_file, vs_file;
   std::string fs_path = std::string(path) + ".fs";
   std::string vs_path = std::string(path) + ".vs";
   fs_file.open(fs_path, std::ios::in | std::ios::binary);
   vs_file.open(vs_path, std::ios::in | std::ios::binary);

   if (!fs_file.is_open() || !vs_file.is_open()) {
      return nullptr;
   }

   auto read_file = [](std::ifstream& stream) -> std::string {
      stream.seekg(0, stream.end);
      std::streampos file_len = stream.tellg();
      stream.seekg(0, stream.beg);
      std::string file_data;
      file_data.resize(static_cast<size_t>(file_len));

      stream.read(file_data.data(), file_data.size());

      return file_data;
   };

   std::string vs_contents = std::move(read_file(vs_file));
   std::string fs_contents = std::move(read_file(fs_file));

   std::unique_ptr<ShaderResource> res_out = std::make_unique<ShaderResource>();

   res_out->load_shader(vs_contents, fs_contents);
   res_out->load_all_uniforms();

   return std::move(res_out);
}

void ShaderResource::load_shader(std::string_view vs_data, std::string_view fs_data) {
   uint32_t fragment_shader, vertex_shader;

   char const* tmp = vs_data.data();
   vertex_shader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex_shader, 1, &tmp, NULL);
   glCompileShader(vertex_shader);

   tmp = fs_data.data();
   fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment_shader, 1, &tmp, NULL);
   glCompileShader(fragment_shader);

   shader_program = glCreateProgram();

   glAttachShader(shader_program, vertex_shader);
   glAttachShader(shader_program, fragment_shader);
   glLinkProgram(shader_program);

   glDeleteShader(vertex_shader);
   glDeleteShader(fragment_shader);
}

void ShaderResource::load_all_uniforms() {
   GLint uniform_count, size;
   std::array<char, 1024> name_tmp;
   name_tmp[1023] = '\0';

   glGetProgramiv(shader_program, GL_ACTIVE_UNIFORMS, &uniform_count);
   for (int i = 0; i < uniform_count; i++) {
      GLsizei name_len;
      GLenum uniform_type;
      glGetActiveUniform(shader_program, static_cast<GLuint>(i), 1023, &name_len, &size,
                         &uniform_type, name_tmp.data());
      std::string uniform_name(name_tmp.data(), name_len);
      uniform_map.emplace(std::move(uniform_name), glGetUniformLocation(shader_program, name_tmp.data()));
   }
}

void ShaderResource::use_program() const { glUseProgram(shader_program); }

void ShaderResource::set_uniform(std::string_view name, float val) const {
   auto it = uniform_map.find(name);
   if (it != uniform_map.end()) {
      glUniform1f(it->second, val);
   }
}

void ShaderResource::set_uniform(std::string_view name, vec2 val) const {
   auto it = uniform_map.find(name);
   if (it != uniform_map.end()) {
      glUniform2f(it->second, val.x, val.y);
   }
}

void ShaderResource::set_uniform_i(std::string_view name, int val) const {
   auto it = uniform_map.find(name);
   if (it != uniform_map.end()) {
      glUniform1i(it->second, val);
   }
}

ShaderResource::~ShaderResource() { glDeleteProgram(shader_program); }
