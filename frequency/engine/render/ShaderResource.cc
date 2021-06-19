#include "ShaderResource.hh"

// clang-format off
#include <gl/glew.h>
#include <SDL_opengl.h>
// clang-format on

ShaderResource::~ShaderResource() { glDeleteProgram(shaderProgram); }

void ShaderResource::load_fragment(std::string_view fragment) {
   char const* tmp = fragment.data();
   fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &tmp, NULL);
   glCompileShader(fragmentShader);
}

void ShaderResource::load_vertex(std::string_view vertex) {
   char const* tmp = vertex.data();
   vertexShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertexShader, 1, &tmp, NULL);
   glCompileShader(vertexShader);
}

void ShaderResource::link_program() {
   shaderProgram = glCreateProgram();

   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glLinkProgram(shaderProgram);

   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);
}

void ShaderResource::use_program() { glUseProgram(shaderProgram); }