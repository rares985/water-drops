#pragma once

#include <Core/Engine.h>

enum ObjectType {
	PLANE, SPHERE, DROP,LIGHT
};

class SceneObject {

	public:
		SceneObject(ObjectType t);
		~SceneObject();

		GLboolean should_render;
		std::string mesh_name;

		glm::vec3 color;
		glm::vec3 scale;
		glm::vec3 position;
		GLfloat time_lived;

		

};