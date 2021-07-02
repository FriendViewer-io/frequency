#pragma once

#include "engine/core/Extension.hh"

#include <array>
#include <vector>

enum ButtonState {
	OFF,
   PRESS,
	HOLD,
	RELEASE,
};

enum MouseButtons : int {
   MOUSE_LEFT,
   MOUSE_MIDDLE,
   MOUSE_RIGHT,
   MOUSE_X1,
   MOUSE_X2,
};

class InputExtension : public Extension {
public:
	void extension_init() override;
	void pre_tick(float dt) override;
	void pre_post_tick(float dt) override {}
	void tick_end(float dt) override {}
	void extension_reset() override {}

   void object_added(GObject* obj) override {}
   void object_removed(GObject* obj) override {}

   std::string_view extension_name() const { return "InputExtension"; }

   constexpr ButtonState get_key_state(int scancode) const { return kb_button_states[scancode]; }
   constexpr bool get_key_down(int scancode) const { return saved_kb_state[scancode] ? true : false; }

   constexpr int get_mouse_x() const { return mouse_x; }
   constexpr int get_mouse_y() const { return mouse_y; }
   constexpr ButtonState get_mouse_state(MouseButtons btn) const { return mouse_button_states[static_cast<int>(btn)]; }
   constexpr bool get_mouse_down(MouseButtons btn) const { return saved_mouse_state & (1 << static_cast<int>(btn)); }

   virtual ~InputExtension() {}

private:
	std::vector<uint8_t> saved_kb_state;
	std::vector<ButtonState> kb_button_states;
   uint32_t saved_mouse_state;
	std::array<ButtonState, 5> mouse_button_states;
   int mouse_x, mouse_y;
};