


#include <Core/GPU/Mesh.h>
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>

#include "SceneObject.h"
#include "macros.h"



class Scene3D : public SimpleScene {
	public:

		Scene3D();
		~Scene3D();

		void Init() override;

	private:

		void FrameStart() override;
		void Update(float frameDuration) override;
		void FrameEnd() override;

		void OnInputUpdate(float frameDuration, int mods) override;
		void OnKeyPress(int keyPressed, int mods) override;

		void InitMeshes();

		Mesh* CreateMesh(const char*, const std::vector<VertexFormat>&, const std::vector<unsigned short>&);
		void Render(SceneObject*);

		Shader* shader;

		SceneObject* plane;
		SceneObject* sphere;
		std::vector<SceneObject*> drops;


		glm::vec3 light_position; /* pozitia sursei de lumina */

		glm::vec2 wave_centers[MAX_WAVES]; /* pozitiile centrelor undelor */
		float wave_creation_time[MAX_WAVES]; /* momentul de timp la care unda a fost creata */

		GLint wave_count;

		GLenum polygon_mode;

		GLboolean wave_trigger;
};