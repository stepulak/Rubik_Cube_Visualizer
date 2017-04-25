#version 330

out vec4 final_color;

in vec3 vertex_position;
in vec3 vertex_normal_vec;

// light
uniform vec4 light_position;
uniform vec3 light_ambient_color;
uniform vec3 light_diffuse_color;
uniform vec3 light_specular_color;
uniform vec3 eye_position;

// fragment material
uniform vec3 material_ambient_color;
uniform vec3 material_diffuse_color;
uniform vec3 material_specular_color;
uniform float material_shininess;

void compute_light(out vec3 light)
{
	vec3 dir_fragment_light = normalize(light_position.xyz - light_position.w * vertex_position);
	vec3 eye = normalize(eye_position - vertex_position);
	vec3 half_eye_light = normalize(0.5 * eye + dir_fragment_light);
	
	float distance = distance(light_position.xyz, vertex_position) * light_position.w;
	float penetration_q = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
	
	// lambert model
	float diffuse_intensity = max(dot(dir_fragment_light, vertex_normal_vec), 0.0) * penetration_q;

	// phong model
	float specular_intensity = pow(max(dot(half_eye_light, vertex_normal_vec), 0.0), material_shininess) * diffuse_intensity;

	vec3 ambient_color = light_ambient_color * material_ambient_color;
	vec3 diffuse_color =  light_diffuse_color * material_diffuse_color * diffuse_intensity;
	vec3 specular_color = light_specular_color * material_specular_color * specular_intensity;

	// return
	light = ambient_color + diffuse_color + specular_color;
}

void main()
{
	vec3 light;
	compute_light(light);
	final_color = vec4(light.xyz, 1.0);
}