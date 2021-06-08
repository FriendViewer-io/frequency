#include <SDL.h>
#undef main
#include <conio.h>

#include "engine/core/Component.hh"
#include "engine/core/GObject.hh"
#include "engine/core/GOList.hh"
#include "engine/core/StateManager.hh"
#include "engine/math/Vector.hh"

class TalkingComponent : public Component {
private:
   int num_ticks = 0;

public:
   void on_tick(float dt) override {
      parent->broadcast_message("munt","aaaaah");

   }

   void on_post_tick(float dt) override {
   }
   
   void on_message(GObject* sender, std::string const& msg) override {
   }
   
   void commit(Component const& from) override {
   }

};

class ListeningComponent : public Component {
private:
   int num_ticks = 0;

public:
   void on_tick(float dt) override {
   }

   void on_post_tick(float dt) override {
   }
   
   void on_message(GObject* sender, std::string const& msg) override {
      printf("%s received message %s from %s\n", parent->get_name().c_str(), msg.c_str(), sender->get_name().c_str());
   }
   
   void commit(Component const& from) override {
   }

};


int main() {
   // SDL_Init(SDL_INIT_VIDEO);
   // SDL_CreateWindow("SampleWindow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500,
   //                  SDL_WINDOW_RESIZABLE);
   // _getch();

   GOList* object_list = statemgr::get_object_list();

   GObject* ob1 = new GObject;
   GObject* ob2 = new GObject;
   GObject* ob3 = new GObject;

   // testing disable_messaging
   ob1->init(vec2(),0,vec2(), "adyam", false, false);
   ob2->init(vec2(),0,vec2(), "nyat", true, false);
   ob3->init(vec2(),0,vec2(), "joe", false, false);

   ob1->add_component(std::make_unique<ListeningComponent>());
   ob2->add_component(std::make_unique<ListeningComponent>());
   ob3->add_component(std::make_unique<TalkingComponent>());

   object_list->add_object(ob1);
   object_list->add_object(ob2);
   object_list->add_object(ob3);

   ob3->add_link(ob1, "munt");
   ob3->add_link(ob2, "munt");


   statemgr::core_game_loop(1.f / 60.f);
}