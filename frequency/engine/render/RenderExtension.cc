#include "engine/render/RenderExtension.hh"

#include "engine/core/StateManager.hh"
#include "engine/core/GObject.hh"
#include "engine/core/GOList.hh"
#include "engine/render/RenderComponent.hh"

#include <IL/il.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <gl/glew.h>

#undef main
// Shader sources
const GLchar* vertexSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord * vec2(1.0, -1.0);
}
)";
const GLchar* fragmentSource = R"(
#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
)";

int main() {

   return 0;
}

void RenderExtension::extension_init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return;
    }
    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000,
                                SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    gl_ctx = SDL_GL_CreateContext(window);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    glewExperimental = GL_TRUE;
    glewInit();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ilInit();

    glViewport(0, 0, 1000, 1000);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1000, 1000, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void RenderExtension::pre_tick(float dt) {
   SDL_Event evt = { 0 };
   SDL_PollEvent(&evt);
   if (evt.type == SDL_QUIT) {
      statemgr::shutdown_game();
   }
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderExtension::tick_end(float dt) {
   for (GObject* go : *statemgr::get_object_list()) {
      RenderComponent const* rc = static_cast<RenderComponent const*>(go->get_component("RenderComponent"));
   }
    // Draw a rectangle from the 2 triangles using 6 indices
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    SDL_GL_SwapWindow(window);
}