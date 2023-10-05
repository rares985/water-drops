
#include "SceneObject.h"
#include "macros.h"


SceneObject::SceneObject(ObjectType type) {
	switch (type) {
		case PLANE:
			mesh_name = "plane";
			color = aquamarine;
			position = glm::vec3(0, 0.1f, 0);
			scale = glm::vec3(1);
			break;
		case SPHERE:
			mesh_name = "sphere";
			color = glm::vec3(1, 1, 1);
			position = glm::vec3(0, 3, 0);
			scale = glm::vec3(0.5f, 0.5f, 0.5f);
			break;
		case DROP:
			mesh_name = "sphere";
			color = glm::vec3(1,1,1);
			scale = glm::vec3(0.3f, 0.3f, 0.3f);
			should_render = true;
			time_lived = 0;
			break;
		case LIGHT:
			mesh_name = "sphere";
			color = glm::vec3(1, 1, 0);
			scale = glm::vec3(0.5f, 0.5f, 0.5f);
		default:
			break;
	}
}

SceneObject::~SceneObject() {

}