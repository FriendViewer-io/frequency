#pragma once

#include "engine/core/EngineResource.hh"

class ShaderResource : public Resource {
public:
    ~ShaderResource();

    void load_fragment(std::string_view fragment);
    void load_vertex(std::string_view vertex);
    void link_program();
    void use_program();

private:
    unsigned int shaderProgram;
    unsigned int fragmentShader;
    unsigned int vertexShader;
};