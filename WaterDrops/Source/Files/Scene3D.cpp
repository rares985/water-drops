#include <Core/Engine.h>
#include <iostream>

#include "Scene3D.h"
#include "macros.h"


Scene3D::Scene3D() {

}

Scene3D::~Scene3D() {

}

void Scene3D::InitMeshes() {

	Mesh* sphere_mesh = new Mesh("sphere");
	sphere_mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	meshes[sphere_mesh->GetMeshID()] = sphere_mesh;

	int planeHeight, planeWidth;
	planeHeight = planeWidth = PLANE_SIZE;

	std::vector<VertexFormat> vertices;
	VertexFormat* new_v;
	for (int i = -planeHeight/2; i < planeHeight/2; i++) {
		for (int j = -planeWidth/2; j < planeWidth/2; j++) {
			new_v = new VertexFormat(glm::vec3((float)i*0.1f, 0.0f, (float)j*0.1f), aquamarine, glm::vec3(0, 1, 0), glm::uvec2(0));
			vertices.push_back(*new_v);
		}
	}
	std::vector<unsigned short> indices;
	for (int i = 0; i < planeHeight - 1; i++) {
		int base = i * planeWidth;
		int next_base = (i + 1)*planeWidth;
		int next_top = next_base + planeWidth;
		for (int j = 0; j < planeWidth; j++) {
			indices.push_back(base + j);
			indices.push_back(next_base + j);
		}
		if (i < (planeHeight - 1)) { /* degenerate triangles */
			indices.push_back(next_top - 1);
			indices.push_back(next_base);
		}
	}


	Mesh* grid_mesh = CreateMesh("plane", vertices, indices);
	meshes["plane"]->SetDrawMode(GL_TRIANGLE_STRIP);
}

void Scene3D::Init() {

	polygon_mode = GL_FILL;

	camera->SetPosition(glm::vec3(0, 8, 10));

	InitMeshes();

	plane = new SceneObject(PLANE);
	
	sphere = new SceneObject(SPHERE);
	

	Shader* shader = new Shader("shader");
	std::string vertexShaderLocation = "Source/Shaders/VertexShader.glsl";
	std::string fragmentShaderLocation = "Source/Shaders/FragmentShader.glsl";
	shader->AddShader(vertexShaderLocation, GL_VERTEX_SHADER);
	shader->AddShader(fragmentShaderLocation, GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	shaders[shader->GetName()] = shader;
	this->shader = shader;

	light_position = glm::vec3(-8, 8, 8);

	wave_count = 0;

}

void Scene3D::FrameStart() {
	glClearColor(0.5f, 0.5f, 0.5f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

void Scene3D::Update(float frame_duration) {

	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(3);
	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);

	Render(plane);
	Render(sphere);

	for (auto drop : drops) {
		if (drop->position.y  < GROUND_LEVEL) {
			if (drop->should_render) {
				drop->should_render = false;
				wave_trigger = true;
				glm::vec2  wave_center = glm::vec2(drop->position.x, drop->position.z);
				wave_centers[wave_count] = wave_center;
				wave_creation_time[wave_count] = (float)Engine::GetElapsedTime();
				wave_count++;
			}
		}
		else {
			drop->time_lived += frame_duration;
			drop->position.y -= 0.025f * 9.8f *  drop->time_lived * drop->time_lived;
		}
		if (drop->should_render) {
			Render(drop);
		}
	}


	SceneObject* light_source = new SceneObject(LIGHT);
	light_source->position = light_position;
	
	Render(light_source);
}

void Scene3D::FrameEnd() {
	
}

void Scene3D::OnInputUpdate(float frame_time, int mods) {

	glm::vec3 up = glm::vec3(0, 1, 0);
	glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();
	glm::vec3 forward = GetSceneCamera()->transform->GetLocalOZVector();
	forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

	GLfloat speed = 6.0f;

	
	if (window->KeyHold(GLFW_KEY_KP_8)) light_position += (-forward)*frame_time*speed;
	if (window->KeyHold(GLFW_KEY_KP_4)) light_position += (-right)*frame_time*speed;
	if (window->KeyHold(GLFW_KEY_KP_ADD)) light_position += (-up)*frame_time*speed;
	if (window->KeyHold(GLFW_KEY_KP_2)) light_position += forward*frame_time*speed;
	if (window->KeyHold(GLFW_KEY_KP_6)) light_position += right*frame_time*speed;
	if (window->KeyHold(GLFW_KEY_KP_SUBTRACT)) light_position += up*frame_time*speed;
	
	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		if (window->KeyHold(GLFW_KEY_W)) sphere->position += (-forward)*frame_time*speed;
		if (window->KeyHold(GLFW_KEY_A)) sphere->position += (-right)*frame_time*speed;
		if (window->KeyHold(GLFW_KEY_Q)) sphere->position += (-up)*frame_time*speed;
		if (window->KeyHold(GLFW_KEY_S)) sphere->position += forward*frame_time*speed;
		if (window->KeyHold(GLFW_KEY_D)) sphere->position += right*frame_time*speed;
		if (window->KeyHold(GLFW_KEY_E)) sphere->position += up*frame_time*speed;
	}
}

void Scene3D::OnKeyPress(int key_pressed, int mods) {
	if (key_pressed == GLFW_KEY_SPACE) {
		SceneObject* new_drop = new SceneObject(DROP);
		new_drop->position = sphere->position;
		drops.push_back(new_drop);
		//plane->position.y -= plane->position.y  < GROUND_LEVEL ? 0 : 0.05f;
	}
	if (key_pressed == GLFW_KEY_H) {
		switch (polygon_mode)
		{
		case GL_POINT:
			polygon_mode = GL_FILL;
			break;
		case GL_LINE:
			polygon_mode = GL_POINT;
			break;
		default:
			polygon_mode = GL_LINE;
			break;
		}
	}
}

void Scene3D::Render(SceneObject* obj) {

	Mesh* mesh = meshes[obj->mesh_name];

	if (!mesh || !shader || !shader->GetProgramID())
		return;
	
	/* Compute the object's model matrix */

	glm::mat4 model_matrix = glm::mat4(1);
	model_matrix = glm::translate(model_matrix, obj->position);
	model_matrix = glm::scale(model_matrix, obj->scale);

	glm::vec3 color = obj->color;


	/* Render the object using the specified program */
	glUseProgram(shader->program);

	/* Send the uniforms to the shader */
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "model_matrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));

	glUniformMatrix4fv(glGetUniformLocation(shader->program, "view_matrix"), 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetViewMatrix()));

	glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(GetSceneCamera()->GetProjectionMatrix()));

	glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));

	glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(GetSceneCamera()->transform->GetWorldPosition()));

	glUniform1i(glGetUniformLocation(shader->program, "wave_trigger"), (wave_trigger ? 1 : 0));

	glUniform1f(glGetUniformLocation(shader->program, "time"), (float)Engine::GetElapsedTime());

	glUniform1i(glGetUniformLocation(shader->program, "is_water"), (obj->mesh_name == "plane" ? 1 : 0));

	float amplitude = 0.15f;
	float pi = (float)AI_MATH_PI;
	float w = (2 * pi) / 2.38f;
	float phi = (5 * pi) / 2.38f;

	/* Uniforms for wave properties */

	glUniform1f(glGetUniformLocation(shader->program, "amplitude"), amplitude);
	glUniform1f(glGetUniformLocation(shader->program, "w"), w);
	glUniform1f(glGetUniformLocation(shader->program, "phi"), phi);

	glUniform1i(glGetUniformLocation(shader->program, "wave_count"),wave_count);
	glUniform2fv(glGetUniformLocation(shader->program, "wave_centers"), wave_count, glm::value_ptr(wave_centers[0]));
	glUniform1fv(glGetUniformLocation(shader->program, "wave_creation_time"), wave_count, wave_creation_time);

	

	/* Uniforms for computing light */

	glUniform1f(glGetUniformLocation(shader->program, "kd"), 0.5f);
	glUniform1f(glGetUniformLocation(shader->program, "ks"), 0.5f);
	glUniform1i(glGetUniformLocation(shader->program, "shininess"), 30);
	glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(light_position));

	/* Draw the object */

	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}


Mesh* Scene3D::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices) {

	/* Create and bind the VAO */
	unsigned int VAO = 0;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	/* Create and bind the VBO */
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/* Send the vertices vector into the VBO buffer */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	/* Create the IBO and bind it */
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	/* Send the indices vector into the IBO buffer */
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	/* Set vertex position attribute */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	/* Set vertex normal attribute */
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	/* Set vertex texture coordinate attribute */
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	/* Set vertex color attribute */
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));

	/* Unbind the VAO */
	glBindVertexArray(0);

	/* Check for OpenGL errors */
	CheckOpenGLError();

	/* Save mesh information into a Mesh object */
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	meshes[name]->vertices = vertices;
	meshes[name]->indices = indices;
	return meshes[name];
}
