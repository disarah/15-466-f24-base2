#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <array>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	// raccoon
	Scene::Transform *raccoon = nullptr;

	Scene::Transform *red_mush0 = nullptr;
	Scene::Transform *red_mush1 = nullptr;

	Scene::Transform *brown_mush0 = nullptr;
	Scene::Transform *brown_mush1 = nullptr;
	Scene::Transform *brown_mush2 = nullptr;
	Scene::Transform *brown_mush3 = nullptr;
	Scene::Transform *brown_mush4 = nullptr;
	Scene::Transform *brown_mush5 = nullptr;
	Scene::Transform *brown_mush6 = nullptr;
	Scene::Transform *brown_mush7 = nullptr;

	const float red_effect_rate = 5.f;
	const float mush_spawn_rate = 3.f;
	
	float flipped = 1.f;	// eaten a bad mushroom
	float timer = red_effect_rate;
	float mush_timer = mush_spawn_rate;

	uint8_t score = 0;
	float speedup = 1.f;

	struct Mushroom {
		Scene::Transform *mushroom = nullptr;
		bool on_screen = false;
	};

	Mushroom mushrooms[10];
	
	glm::vec2 raccoon_bbox;
	glm::vec2 mush_bbox;

	glm::quat raccoon_rotation;
	glm::quat mush_rotation;
	float wobble = 0.0f;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
