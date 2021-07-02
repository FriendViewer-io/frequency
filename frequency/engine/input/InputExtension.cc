#include "engine/input/InputExtension.hh"

#include <SDL.h>

void InputExtension::extension_init() {
    SDL_Init(SDL_INIT_EVENTS);
    mouse_x = mouse_y = 0;
    saved_mouse_state = 0;
}

void InputExtension::pre_tick(float dt) {
    SDL_PumpEvents();

    int num_keys;
    const Uint8* cur_kb_state = SDL_GetKeyboardState(&num_keys);

    if (saved_kb_state.size() != num_keys) {
        saved_kb_state.resize(num_keys);
        kb_button_states.resize(num_keys, OFF);
        std::copy(cur_kb_state, cur_kb_state + num_keys, saved_kb_state.data());
        std::fill(mouse_button_states.begin(), mouse_button_states.end(), OFF);

        saved_mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
        return;
    }

    for (int i = 0; i < num_keys; i++) {
        if (!cur_kb_state[i] && !saved_kb_state[i]) {
            kb_button_states[i] = OFF;
        } else if (!cur_kb_state[i] && saved_kb_state[i]) {
            kb_button_states[i] = RELEASE;
        } else if (cur_kb_state[i] && !saved_kb_state[i]) {
            kb_button_states[i] = PRESS;
        } else if (cur_kb_state[i] && saved_kb_state[i]) {
            kb_button_states[i] = HOLD;
        }
    }

    const uint32_t cur_mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
    
    for (int i = 0; i < 5; i++) {
        const int cur_button_i = cur_mouse_state & (1 << i);
        const int saved_button_i = saved_mouse_state & (1 << i);
        if (!cur_button_i && !saved_button_i) {
            mouse_button_states[i] = OFF;
        } else if (!cur_button_i && saved_button_i) {
            mouse_button_states[i] = RELEASE;
        } else if (cur_button_i && !saved_button_i) {
            mouse_button_states[i] = PRESS;
        } else if (cur_button_i && saved_button_i) {
            mouse_button_states[i] = HOLD;
        }
    }

    std::copy(cur_kb_state, cur_kb_state + num_keys, saved_kb_state.data());
    saved_mouse_state = cur_mouse_state;
}