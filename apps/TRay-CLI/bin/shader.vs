#version 330 core                   // version and profile

layout(location = 0) in vec3 v_pos;  // layout of input data
layout(location = 1) in vec2 v_tex_coord;

out vec2 tex_coord;

void main() {
  gl_Position = vec4(v_pos, 1.0);
  tex_coord = v_tex_coord;
}