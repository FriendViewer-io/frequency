#version 330 core

out vec4 frag_color;

in vec2 tex_coord;

uniform sampler2D image_tex;
uniform float somecolor;

void main() {
   frag_color = texture(image_tex, tex_coord);
}