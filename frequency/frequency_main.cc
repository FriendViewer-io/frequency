#include <SDL.h>
#undef main
#include <conio.h>

#include "engine/core/Component.hh"
#include "engine/core/GObject.hh"
#include "engine/core/GOList.hh"
#include "engine/core/StateManager.hh"
#include "engine/math/Vector.hh"
#include "engine/physics/ColliderComponent.hh"


int main() {
   // SDL_Init(SDL_INIT_VIDEO);
   // SDL_CreateWindow("SampleWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500,
   //                  SDL_WINDOW_RESIZABLE);
   // _getch();

   GOList* object_list = statemgr::get_object_list();

   GObject* ob1 = new GObject;
   

   // testing disable_messaging
   ob1->init(vec2(), 0, vec2(), "adyam", false, false);
   ob1->add_component(std::make_unique<ColliderComponent>(vec2(0, 0), vec2(1, 1)));
   object_list->add_object(ob1);


   statemgr::core_game_loop(1.f / 60.f);
}