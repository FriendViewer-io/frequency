#version 330 core
layout (location = 0) in vec2 v_position;
layout (location = 1) in vec2 v_tex_coord;

// Vertices should always be:
// (-1, -1) -> bottom left
// (1, -1) -> bottom right
// (-1, 1) -> top left
// (1, 1) -> top right

out vec2 sprite_tex_coord;

// specified in pixels
uniform vec2 window_dims;
uniform vec2 sheet_dims;
uniform vec2 sprite_clip_min;
uniform vec2 sprite_clip_max;

uniform vec2 position;
// offset from the center of the image (in pixels)
uniform vec2 anchor_offset;
// rotates about origin + anchor_offset
uniform float rotation;

void main() {
   // in pixels
   vec2 sprite_dims = sprite_clip_max - sprite_clip_min;
   vec2 half_dims = sprite_dims / 2;
   vec2 pixel_offset = half_dims * v_position - anchor_offset;

   float sin_a = sin(rotation), cos_a = cos(rotation);
   mat2 rot_m = mat2(cos_a, -sin_a, sin_a, cos_a);
   vec2 rotated_pixel_offset = rot_m * pixel_offset;
   vec2 normalized_pixel_offset = (rotated_pixel_offset / window_dims) * 2;
   vec2 normalized_anchor_offset = (anchor_offset / window_dims) * 2;

   vec2 normalized_position = (position / window_dims) * 2 - vec2(1, 1);

   gl_Position = vec4(normalized_position + normalized_pixel_offset + normalized_anchor_offset, 0.0, 1.0);

   vec2 norm_min = (sprite_clip_min / sheet_dims);
   norm_min = vec2(norm_min.x, 1 - norm_min.y);
   vec2 norm_dims = sprite_dims / sheet_dims;
   vec2 bottom_left = norm_min - vec2(0, norm_dims.y);

   tex_coord = bottom_left + v_tex_coord * norm_dims;
}