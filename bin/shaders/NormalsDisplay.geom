#version 330

// type of input primitives
layout(triangles) in;

// type of output primitives and maximum number of new vertices
// each line requires two vertices
layout(line_strip, max_vertices = 6) out;

///////////////////////////////////////////////////////////////////

// data from vertex shader for three vertices of the triangle
in vec3 o_normal[];

// matrices
uniform mat4 view;
uniform mat4 projection;

// modifier for size of the normals, tangents and bitangents
float u_normalScale = 1.0;

// colors for different type of new lines
vec4 u_edgeColor = vec4(1.0, 0.0, 0.0, 1.0);
vec4 u_faceNormalColor = vec4(1.0, 1.0, 0.0, 1.0);
vec4 u_normalColor = vec4(0.0, 1.0, 1.0, 1.0);

// color to fragment shader
out vec4 o_color;

///////////////////////////////////////////////////////////////////

void main()
{
   mat4 viewProjection = projection * view;

   // normals of each vertex of the triangle
   vec3 nor[3];
   nor[0] = o_normal[0].xyz;
   nor[1] = o_normal[1].xyz;
   nor[2] = o_normal[2].xyz;

   // positions of each vertex of the triangle
   // shifted a bit along normal
   // so there won't be Z fighting when rendered over the mesh
   vec4 pos[3];
   pos[0] = viewProjection * vec4(gl_in[0].gl_Position.xyz + nor[0] * 0.01, 1.0);
   pos[1] = viewProjection * vec4(gl_in[1].gl_Position.xyz + nor[1] * 0.01, 1.0);
   pos[2] = viewProjection * vec4(gl_in[2].gl_Position.xyz + nor[2] * 0.01, 1.0);

   // create edges for triangle
   o_color = u_edgeColor;
   gl_Position = pos[0];
   EmitVertex();
   gl_Position = pos[1];
   EmitVertex();
   gl_Position = pos[2];
   EmitVertex();
   gl_Position = pos[0];
   EmitVertex();
   // end line strip after four added vertices, so we will get three lines
   EndPrimitive();

   // create normal for triangle
   o_color = u_faceNormalColor;
   // form two vectors from triangle
   vec3 V0 = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
   vec3 V1 = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;
   // calculate normal as perpendicular to two vectors of the triangle
   vec3 N = normalize(cross(V1, V0));
   // position as arithmetic average
   vec3 P = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz
                        + gl_in[2].gl_Position.xyz) / 3.0;
   gl_Position = viewProjection * vec4(P, 1.0);
   EmitVertex();
   gl_Position = viewProjection * vec4(P + N * u_normalScale, 1.0);
   EmitVertex();
   EndPrimitive();
}