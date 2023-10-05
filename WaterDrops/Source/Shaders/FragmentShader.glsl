#version 330


in vec3 world_normal;
in vec3 world_position;

in vec3 frag_color;

uniform vec3 light_position;
uniform vec3 eye_position;

uniform float ks;
uniform float kd;
uniform int shininess;



layout(location = 0) out vec4 output_color;


void main() 
{
	vec3 L = normalize(light_position - world_position);
	vec3 V = normalize(eye_position - world_position);
	vec3 H = normalize(L + V);
	vec3 N = normalize(world_normal);

	float ambient_light = 0.35f;

	float diffuse_light = kd * max(dot(world_normal,L),0);

	float specular_light = 0;

	if (diffuse_light > 0) {
		specular_light = ks * pow(max(dot(world_normal,H),0),shininess);
	}

	float attenuation = 1.0f / (1.0f + 0.005f * pow(distance(light_position,world_position),2));

	float light = ambient_light + attenuation*(specular_light + diffuse_light);




	output_color = vec4(vec3(frag_color)* light,1.0);
}