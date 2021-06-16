
#include <conio.h>
#include <thread>
#include <SDL.h>

#undef main

#include "engine/core/Component.hh"
#include "engine/core/Extension.hh"
#include "engine/core/GOList.hh"
#include "engine/core/GObject.hh"
#include "engine/core/StateManager.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/ColliderComponent.hh"
#include "engine/physics/CircleCollider.hh"
#include "engine/physics/PhysicsExtension.hh"
#include "engine/physics/Quadtree.hh"
#include "engine/physics/CellPartition.hh"
#include <Windows.h>


void draw_vec(SDL_Renderer* r, vec2 p1, vec2 p2) {
   SDL_RenderDrawLineF(r, p1.x, p1.y, p2.x, p2.y);
}

void draw_circle(SDL_Renderer* renderer, float radius, float x, float y, int iterations) {
   vec2 first_point = vec2(radius, 0);
   const vec2 center = vec2(x, y);

   constexpr float total_angle = 2 * 3.141592654f;
   const float step_angle = total_angle / iterations;

   for (int i = 0; i < iterations; i++) {
      vec2 second_point = first_point.rotated(step_angle);

      vec2 fp = first_point + center;
      vec2 sp = second_point + center;

      SDL_RenderDrawLineF(renderer, fp.x, fp.y, sp.x, sp.y);
      first_point = second_point;
   }
}

void draw_box(SDL_Renderer* renderer, aabb const& bounds) {
   vec2 tr = vec2(bounds.max.x, bounds.min.y);
   vec2 bl = vec2(bounds.min.x, bounds.max.y);

   draw_vec(renderer, bounds.min, tr);
   draw_vec(renderer, tr, bounds.max);
   draw_vec(renderer, bounds.max, bl);
   draw_vec(renderer, bl, bounds.min);
}

class GhettoExtension : public Extension {
public:
   void extension_init() override {
      SDL_Init(SDL_INIT_VIDEO);
      window = SDL_CreateWindow("SampleWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000,
                                1000, SDL_WINDOW_RESIZABLE);

      renderer = SDL_CreateRenderer(window, 0, SDL_RendererFlags::SDL_RENDERER_ACCELERATED);
   }
   void pre_tick(float dt) override {
      SDL_Event evt = {0};
      SDL_PollEvent(&evt);
      if (evt.type == SDL_QUIT) {
         statemgr::shutdown_game();
      }
   }
   // Detect [& solve collisions]
   void pre_post_tick(float dt) override {}
   void tick_end(float dt) override {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
      
      auto& quad_part = static_cast<Quadtree&>(statemgr::get_physics_extension()->get_static_partition());
      auto& cell_part = static_cast<CellPartition&>(statemgr::get_physics_extension()->get_dynamic_partition());

      SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
      quad_part.for_each_level(
          [this](aabb const& aabb) {
         draw_box(renderer, aabb);
      });

      aabb world_bounds = cell_part.get_world_bounds();
      vec2 cell_dimensions = cell_part.get_cell_dimensions();

      int num_cells_width =
       static_cast<int>(ceilf((world_bounds.max.x - world_bounds.min.x) / cell_dimensions.x));
      int num_cells_height =
       static_cast<int>(ceilf((world_bounds.max.y - world_bounds.min.y) / cell_dimensions.y));

      SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
      for (int i = 0; i < num_cells_height; i++) {
         for (int j = 0; j < num_cells_width; j++) {
            vec2 cell_top_left = world_bounds.min + vec2(0, cell_dimensions.y * i) + vec2(cell_dimensions.x * j, 0);
            vec2 cell_top_right = cell_top_left + vec2(cell_dimensions.x, 0);
            vec2 cell_bottom_left = cell_top_left + vec2(0, cell_dimensions.y);

            draw_vec(renderer, cell_top_left, cell_top_right);
            draw_vec(renderer, cell_top_left, cell_bottom_left);
         }
      }

      for (GObject* obj : *statemgr::get_object_list()) {
         auto* collider = obj->get_component("ColliderComponent");
         if (collider != nullptr) {
            CircleCollider const* cc = static_cast<CircleCollider const*>(collider);

            if (cc->is_static()) {
               SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            } else {
               SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            }

            draw_circle(renderer, cc->get_radius(), obj->get_position().x, obj->get_position().y, 3);
         }
      }


      int mx, my;
      SDL_GetMouseState(&mx, &my);
      for (GObject* obj : *statemgr::get_object_list()) {
         auto* collider = obj->get_staging_component("ColliderComponent");
         if (collider != nullptr) {
            CircleCollider* cc = static_cast<CircleCollider*>(collider);
            aabb cursor_area = aabb(vec2(mx - 1, my - 1), vec2(mx + 1, my + 1));

            aabb collider_area = cc->bounding_box();
            collider_area.shift(obj->get_staging_position());

            if (collider_area.overlap(cursor_area)) {
               // run a query on this object
               std::vector<ColliderComponent*> colliders;
               quad_part.query(collider_area, colliders);
               int static_sz = colliders.size();
               SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
               for (ColliderComponent* cc2 : colliders) {
                  aabb cc2_area = cc2->bounding_box();
                  cc2_area.shift(cc2->get_parent()->get_staging_position());

                  draw_box(renderer, cc2_area);
               }

               colliders.clear();
               
               cell_part.query(collider_area, colliders);
               SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
               for (ColliderComponent* cc2 : colliders) {
                  aabb cc2_area = cc2->bounding_box();
                  cc2_area.shift(cc2->get_parent()->get_staging_position());

                  draw_box(renderer, cc2_area);
               }
               printf("Num static queried: %d, dynamic: %d\n", static_sz, colliders.size());
            }
         }
      }

      SDL_RenderPresent(renderer);
   }
   void extension_reset() override {}

   std::string_view extension_name() const { return "GhettoExtension"; }

private:
   SDL_Renderer* renderer;
   SDL_Window* window;
};

int main() {
   GhettoExtension ext;

   srand(time(NULL));
   for (int i = 0; i < 10; i++) {
      statemgr::reset_game();
      GObject* n = new GObject;
      statemgr::get_object_list()->add_object(n);
      float rx = rand() % 980 + 10;
      float ry = rand() % 980 + 10;

      if (i == 0) {
         n->init(vec2(500, 500), 0, vec2(1, 1), std::to_string(i), false, false);
      } else 
      n->init(vec2(rx, ry), 0, vec2(1, 1), std::to_string(i), false, false);
      if (i < 4) {
         n->add_component(std::make_unique<CircleCollider>(false, vec2(0, 0), vec2(), 30));
      } else {
         n->add_component(std::make_unique<CircleCollider>(true, vec2(20, 20), vec2(20, 20), 2));
      }
   }

   statemgr::add_extension(new GhettoExtension());
   statemgr::core_game_loop(1.f / 60.f);
}


// #include <gl/glew.h>

// #include <SDL_opengl.h>
// #include <SDL.h>

// #undef main
// // Shader sources
// const GLchar* vertexSource = R"glsl(
//     #version 150 core
//     in vec2 position;
//     uniform vec3 color;
//     out vec3 Color;
//     void main()
//     {
//         Color = color;
//         gl_Position = vec4(position, 0.0, 1.0);
//     }
// )glsl";
// const GLchar* fragmentSource = R"glsl(
//     #version 150 core
//     in vec3 Color;
//     out vec4 outColor;
//     void main()
//     {
//         outColor = vec4(Color, 1.0);
//     }
// )glsl";

// int main()
// {


    
    
    
//     if (SDL_Init(SDL_INIT_VIDEO) < 0) /* Initialize SDL's Video subsystem */
//         return 1;//sdldie("Unable to initialize SDL"); /* Or die on error */
//    SDL_Window* window = SDL_CreateWindow("SampleWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000,
//                            1000, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

//    SDL_GLContext maincontext = SDL_GL_CreateContext(window);

//     /* Request opengl 3.2 context.
//      * SDL doesn't have the ability to choose which profile at this time of writing,
//      * but it should default to the core profile */
//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//     SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

//     /* Turn on double buffering with a 24bit Z buffer.
//      * You may need to change this to 16 or 32 for your system */
//     SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//     SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

//     // Initialize GLEW
//     glewExperimental = GL_TRUE;
//     glewInit();

//     // Create Vertex Array Object
//     GLuint vao;
//     glGenVertexArrays(1, &vao);
//     glBindVertexArray(vao);

//     // Create a Vertex Buffer Object and copy the vertex data to it
//     GLuint vbo;
//     glGenBuffers(1, &vbo);

//     GLfloat vertices[] = {
//         -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
//          0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
//          0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
//         -0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
//     };

//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     // Create an element array
//     GLuint ebo;
//     glGenBuffers(1, &ebo);

//     GLuint elements[] = {
//         0, 1, 2
//         //2, 3, 0
//     };

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

//     // Create and compile the vertex shader
//     GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexSource, NULL);
//     glCompileShader(vertexShader);

//     // Create and compile the fragment shader
//     GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
//     glCompileShader(fragmentShader);

//     // Link the vertex and fragment shader into a shader program
//     GLuint shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     //
//     glBindFragDataLocation(shaderProgram, 0, "outColor");
//     glLinkProgram(shaderProgram);
//     glUseProgram(shaderProgram);

//     // Specify the layout of the vertex data
//     GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
//     glEnableVertexAttribArray(posAttrib);
//     glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

//     GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
//     glEnableVertexAttribArray(colAttrib);
//     glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

//     bool running = true;
//     while (running)
//     {
//         /*sf::Event windowEvent;
//         while (window.pollEvent(windowEvent))
//         {
//             switch (windowEvent.type)
//             {
//             case sf::Event::Closed:
//                 running = false;
//                 break;
//             }
//         }*/

//         // Clear the screen to black
//         glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);

//         // Draw a rectangle from the 2 triangles using 6 indices
//         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//         // Swap buffers
//         //window.display();
//         SDL_GL_SwapWindow(window);
//     }

//     glDeleteProgram(shaderProgram);
//     glDeleteShader(fragmentShader);
//     glDeleteShader(vertexShader);

//     glDeleteBuffers(1, &ebo);
//     glDeleteBuffers(1, &vbo);

//     glDeleteVertexArrays(1, &vao);

//     return 0;
// }
