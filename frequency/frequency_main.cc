#include <SDL.h>
#undef main
#include <conio.h>

#include "engine/core/Component.hh"
#include "engine/core/GObject.hh"
#include "engine/core/GOList.hh"
#include "engine/core/StateManager.hh"
#include "engine/math/Vector.hh"

class WalkingComponent : public Component {
public:
   int counter = 0;

   void on_tick(float dt) override {
      counter++;
      parent_data->position.x += 1.f;

      printf("My position: %f, counter %d\n", parent_data->position.x, counter);
   }

   void on_post_tick(float dt) override {
   }
   
   void on_message(GObject* sender, std::string const& msg) override {
   }
   
   void commit(Component const& from) override {
      WalkingComponent const& rhs = static_cast<WalkingComponent const&>(from);
      counter = rhs.counter;
   }

   std::unique_ptr<Component> clone() override {
      return std::make_unique<WalkingComponent>();
   }
   
   std::string_view get_component_type_name() const {
      return "WalkingComponent";
   }

};

class ObserverComponent : public Component {
private:
   int num_ticks = 0;

public:
   void on_tick(float dt) override {
      GObject* go = statemgr::get_object_list()->get_object("adyam");
      WalkingComponent const* wc = static_cast<WalkingComponent const*>(go->get_component("WalkingComponent"));

      printf("Walking guy's position: %f, counter %d\n", go->get_position().x, wc->counter);
   }

   void on_post_tick(float dt) override {
   }
   
   void on_message(GObject* sender, std::string const& msg) override {
      printf("%s received message %s from %s\n", parent->get_name().c_str(), msg.c_str(), sender->get_name().c_str());
   }
   
   void commit(Component const& from) override {
   }

   std::unique_ptr<Component> clone() override { return std::make_unique<ObserverComponent>(); }
   
   std::string_view get_component_type_name() const {
      return "ObserverComponent";
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

   // testing disable_messaging
   ob1->init(vec2(), 0, vec2(), "adyam", false, false);
   ob2->init(vec2(), 0, vec2(), "nyat", false, false);

   ob1->add_component(std::make_unique<WalkingComponent>());
   ob2->add_component(std::make_unique<ObserverComponent>());

   object_list->add_object(ob1);
   object_list->add_object(ob2);
   
   statemgr::core_game_loop(1.f / 60.f);
}