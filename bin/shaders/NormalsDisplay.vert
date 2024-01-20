#version 330

// attributes
layout(location = 0) in vec3 pos; // xyz - position
layout(location = 1) in vec3 norm; // xyz - normal
layout(location = 2) in vec4 i_tangent; // xyz - tangent, w - handedness

// matrices
uniform mat4 model;
mat3 u_normal_mat = mat3(1.0);

// data to geometry shader
out vec3 o_normal;

///////////////////////////////////////////////////////////////////

void main(void)
{
   // position to world coordinates
   gl_Position = model * vec4(pos, 1.0);

   // normal, tangent and bitangent in world coordinates
   o_normal = normalize(u_normal_mat * norm);
} 