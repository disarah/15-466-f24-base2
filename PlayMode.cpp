#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint hexapod_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > hexapod_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("forest.pnct"));
	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("forest.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = hexapod_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

PlayMode::PlayMode() : scene(*hexapod_scene) {
	//https://en.cppreference.com/w/cpp/numeric/random/rand#:~:text=If%20rand()%20is%20used,of%20values%20on%20successive%20calls.
	std::srand(std::time(nullptr)); 

	for (auto &transform : scene.transforms) {
		if (transform.name == "Raccoon") raccoon = &transform;
		else if (transform.name == "RedMush") red_mush0 = &transform;
		else if (transform.name == "RedMush.001") red_mush1 = &transform;
		else if (transform.name == "BrownMush") brown_mush0 = &transform;
		else if (transform.name == "BrownMush.001") brown_mush1 = &transform;
		else if (transform.name == "BrownMush.002") brown_mush2 = &transform;
		else if (transform.name == "BrownMush.003") brown_mush3 = &transform;
		else if (transform.name == "BrownMush.004") brown_mush4 = &transform;
		else if (transform.name == "BrownMush.005") brown_mush5 = &transform;
		else if (transform.name == "BrownMush.006") brown_mush6 = &transform;
		else if (transform.name == "BrownMush.007") brown_mush7 = &transform;

	}
	if (raccoon == nullptr) throw std::runtime_error("Raccoon not found.");

	if (red_mush0 == nullptr) throw std::runtime_error("Red Mushroom not found.");
	if (red_mush1 == nullptr) throw std::runtime_error("Red Mushroom not found.");

	if (brown_mush0 == nullptr) throw std::runtime_error("Brown Mushroom not found.");
	if (brown_mush1 == nullptr) throw std::runtime_error("Brown Mushroom not found.");
	if (brown_mush2 == nullptr) throw std::runtime_error("Brown Mushroom not found.");
	if (brown_mush3 == nullptr) throw std::runtime_error("Brown Mushroom not found.");
	if (brown_mush4 == nullptr) throw std::runtime_error("Brown Mushroom not found.");
	if (brown_mush5 == nullptr) throw std::runtime_error("Brown Mushroom not found.");
	if (brown_mush6 == nullptr) throw std::runtime_error("Brown Mushroom not found.");
	if (brown_mush7 == nullptr) throw std::runtime_error("Brown Mushroom not found.");

	raccoon_bbox = glm::vec2(0.25f);
	mush_bbox = glm::vec2(0.1f,0.1f); 		// use for all mushrooms

	red_mush0->position = glm::vec3(1.f,0.f,0.f);
	brown_mush0->position = glm::vec3(-1.f,0.f,0.f);

	mushrooms[0].mushroom = red_mush0;
	mushrooms[1].mushroom = brown_mush0;
	mushrooms[2].mushroom = brown_mush1;
	mushrooms[3].mushroom = brown_mush2;
	mushrooms[4].mushroom = brown_mush3;
	mushrooms[5].mushroom = red_mush1;
	mushrooms[6].mushroom = brown_mush4;
	mushrooms[7].mushroom = brown_mush5;
	mushrooms[8].mushroom = brown_mush6;
	mushrooms[9].mushroom = brown_mush7;

	mushrooms[0].on_screen = true;
	mushrooms[1].on_screen = true;

	raccoon_rotation = raccoon->rotation;
	mush_rotation = red_mush0->rotation; 		// use for all mushrooms

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	wobble += elapsed / 10.0f;
	wobble -= std::floor(wobble);

	for (Mushroom m : mushrooms){
		if(m.on_screen){
			m.mushroom->rotation = mush_rotation * glm::angleAxis(glm::radians(-360.f * wobble), glm::vec3(0.0f, 0.0f, 1.0f));
		}
	}

	{
		//Make inputs move around raccoon
		float PlayerSpeed = 30.0f * speedup;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) {
			move.x =-1.0f * flipped;
			raccoon->rotation = raccoon_rotation * glm::angleAxis(glm::radians(-90.f),glm::vec3(0.0f, 0.0f, 1.0f));
		}
		if (!left.pressed && right.pressed) {
			move.x = 1.0f * flipped;
			raccoon->rotation = raccoon_rotation * glm::angleAxis(glm::radians(90.f),glm::vec3(0.0f, 0.0f, 1.0f));
		}
		if (down.pressed && !up.pressed) {
			move.y =-1.0f * flipped;
			raccoon->rotation = raccoon_rotation * glm::angleAxis(glm::radians(0.f),glm::vec3(0.0f, 0.0f, 1.0f));
		}
		if (!down.pressed && up.pressed) {
			move.y = 1.0f * flipped;
			raccoon->rotation = raccoon_rotation * glm::angleAxis(glm::radians(180.f),glm::vec3(0.0f, 0.0f, 1.0f));
		}

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		glm::mat4x3 frame = raccoon->make_local_to_parent();
		glm::vec3 frame_right = frame[0] * raccoon->rotation;
		glm::vec3 up = frame[1] * raccoon->rotation;

		raccoon->position += move.x * frame_right + move.y * up;

		// clamp to within grassy area
		raccoon->position.x = std::clamp(raccoon->position.x, -3.2f, 3.2f);
		raccoon->position.y = std::clamp(raccoon->position.y, -3.f, 2.2f);
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	// check if raccoon collides with any mushrooms
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
	// barely a bbox, more like a bsquare

	float rminX = raccoon->position.x - raccoon_bbox.x;
	float rmaxX = raccoon->position.x + raccoon_bbox.x;
	float rminY = raccoon->position.y - raccoon_bbox.y;
	float rmaxY = raccoon->position.y + raccoon_bbox.y;

	for (uint32_t i = 0; i < 10; i++) {
		Mushroom m = mushrooms[i];
		float mminX = m.mushroom->position.x - mush_bbox.x;
		float mmaxX = m.mushroom->position.x + mush_bbox.x;
		float mminY = m.mushroom->position.y - mush_bbox.y;
		float mmaxY = m.mushroom->position.y + mush_bbox.y;
		if (mminX <= rmaxX && mmaxX >= rminX && mminY <= rmaxY && mmaxY >= rminY && mushrooms[i].on_screen){ // move mushroom out of frame
			mushrooms[i].on_screen = false;
			mushrooms[i].mushroom->position = glm::vec3(5.f, 5.f, 0.f);
			if(i%5 == 0) { // if red mushroom set flipped to -1
				flipped = -1;
				timer = red_effect_rate;
				score +=3;
				speedup = std::max(speedup - 0.3f, 0.01f);
			} else {
				speedup = std::max(speedup + 0.05f, 0.01f);
				score +=1;
			}
		}
	}

	// red mushroom effects
	if(flipped == -1) timer = std::max(timer - elapsed, 0.f);
	if(timer<=0.f) flipped = 1;

	// add a new mushroom to screen every x seconds
	mush_timer = std::max(mush_timer - elapsed, 0.f);

	if(mush_timer <= 0.f) {
		for(uint32_t j = 0; j < 10; j++){ // find first off screen mushroom
			if(!mushrooms[j].on_screen){
				
				mushrooms[j].on_screen = true;
				// https://stackoverflow.com/questions/686353/random-float-number-generation
				mushrooms[j].mushroom->position.x = -3.2f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(6.4f)));
				mushrooms[j].mushroom->position.y = -3.f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(5.2f)));
				break;
			}
		}
		mush_timer = mush_spawn_rate;
	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.f, 1.0f, 0.f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	GL_ERRORS(); //print any errors produced by this setup code

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("W A S D score: " + std::to_string(score),
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
}
