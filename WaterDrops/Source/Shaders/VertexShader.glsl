#version 330

/* Vertex properties */
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture_coordinate;
layout(location = 3) in vec3 vertex_color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform vec3 object_color;

float GROUND_LEVEL = 1.0f;


/* Uniforms for wave properties */
uniform float amplitude;
uniform float time;
uniform float phi;
uniform float w;
uniform int is_water;
uniform int wave_trigger;
uniform int wave_count;
uniform float wave_lifetime[300];
uniform vec2 wave_centers[300];


out vec3 frag_color;


out vec3 world_normal;
out vec3 world_position;


void main() 
{

	vec3 pos = vertex_position;

	float Hdx = 0; /* derivata partiala dupa x a functiei H */
	float Hdz = 0; /* derivata partiala dupa z a functiei H */

	float vertex_height = vertex_position.y;
	float Wdx,Wdz;

	if (is_water > 0) {
		if (wave_trigger > 0 ) {
			for(int i = 0; i < wave_count; i ++) {

				float lifetime = time - wave_lifetime[i]; /* durata de viata a undei */
				
				vec2 xz = vertex_position.xz - wave_centers[i];

				vec2 direction = normalize(wave_centers[i] - vertex_position.xz);
					
				float dot_product = dot(direction,xz);

				float damping_factor = pow(2.71f, - 0.1f * lifetime);

				vertex_height += (damping_factor * amplitude * sin(dot_product * w + lifetime * phi));


				Wdx = damping_factor * amplitude * direction.x * w * cos(dot_product * w + lifetime * phi);
				Wdz = damping_factor * amplitude * direction.y * w * cos(dot_product * w + lifetime * phi); 


				Hdx += Wdx;
				Hdz += Wdz;
	
			}
			pos = vec3(vertex_position.x, vertex_height, vertex_position.z);
		}
	}


	world_position = (model_matrix * vec4(pos,1.0f)).xyz;
	world_normal = normalize(vec3(-Hdx,1,-Hdz));


	frag_color = object_color;

	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(pos,1.0f);
}

