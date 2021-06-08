#include <SDL.h>
#undef main
#include <conio.h>

#include "engine/core/Component.hh"
#include "engine/core/GObject.hh"
#include "engine/core/GOList.hh"
#include "engine/core/StateManager.hh"
#include "engine/math/Vector.hh"

class NatComponent : public Component {
private:
   int hunger = 0;
public:
   void on_tick(float dt) override {
      hunger++;
      if (hunger > 100) {
         parent->broadcast_message("hungry", "I am going to chipotle");
         hunger = 0;
         parent->broadcast_message("full", "mm tasty food");
      }
   }

   void on_post_tick(float dt) override {
   }
   
   void on_message(GObject* sender, std::string const& msg) override {
   }
   
   void commit(Component const& from) override {

   }

};

class AnyoneComponent : public Component {
public:
   void on_tick(float dt) override {
   }

   void on_post_tick(float dt) override {
   }
   
   void on_message(GObject* sender, std::string const& msg) override {
      printf("%s received a message from %s: %s\n", parent->get_name().c_str(), sender->get_name().c_str(), msg.c_str());
   }
   
   void commit(Component const& from) override {

   }

};


int main() {
   // SDL_Init(SDL_INIT_VIDEO);
   // SDL_CreateWindow("SampleWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500,
   //                  SDL_WINDOW_RESIZABLE);
   // _getch();

   GOList* list = statemgr::get_object_list();
   
   GObject* nat_go = new GObject();
   GObject* andrew_go = new GObject();
   GObject* sam_go = new GObject();
   GObject* hzr_go = new GObject();
   
   nat_go->init(vec2(0, 0), 0, vec2(1, 1), "nat");
   andrew_go->init(vec2(0, 0), 0, vec2(1, 1), "andrew");
   sam_go->init(vec2(0, 0), 0, vec2(1, 1), "sam");
   hzr_go->init(vec2(0, 0), 0, vec2(1, 1), "hzr");
   
   nat_go->add_component(std::move(std::make_unique<NatComponent>()));
   andrew_go->add_component(std::move(std::make_unique<AnyoneComponent>()));
   sam_go->add_component(std::move(std::make_unique<AnyoneComponent>()));
   hzr_go->add_component(std::move(std::make_unique<AnyoneComponent>()));

   list->add_object(nat_go);
   list->add_object(andrew_go);
   list->add_object(sam_go);
   list->add_object(hzr_go);

   nat_go->add_link(andrew_go, "hungry");
   nat_go->add_link(andrew_go, "full");
   nat_go->add_link(sam_go, "hungry");
   nat_go->add_link(sam_go, "full");
   nat_go->add_link(hzr_go, "hungry");
   nat_go->add_link(hzr_go, "full");

   statemgr::core_game_loop(1.f / 60.f);
}