#version 330

in vec4 position;
in vec3 normal;

out vec3 vertex_position;
out vec3 vertex_normal_vec;

uniform mat4 pvm_matrix;
uniform mat3 normal_matrix;
uniform mat4 model_matrix; // for vertex position in world space

void main()
{
	vertex_normal_vec = normalize(normal_matrix * normal);
	vertex_position = (model_matrix * position).xyz;
	gl_Position = pvm_matrix * position;
}
