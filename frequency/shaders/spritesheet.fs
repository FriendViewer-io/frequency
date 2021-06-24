#version 330 core

out vec4 frag_color;

in vec2 sprite_tex_coord;

uniform sampler2D image_tex;

void main() {
   frag_color = texture(image_tex, sprite_tex_coord);
}