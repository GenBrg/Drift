#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {
	ppu.background_color = glm::u8vec4(0x00, 0xFF, 0x00, 0xff);

	for (int i = 0; i < PPU466::BackgroundHeight; ++i) {
		for (int j = 0; j < PPU466::BackgroundWidth; ++j) {
			int idx = i * PPU466::BackgroundWidth + j;
			ppu.background[idx] = PPU466::MakeBackgroundValue(idx % 8, idx % 2);
		}
	}
}

PlayMode::~PlayMode() { }

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	if (left.pressed) player_at.x -= player_speed * elapsed;
	if (right.pressed) player_at.x += player_speed * elapsed;
	if (down.pressed) player_at.y -= player_speed * elapsed;
	if (up.pressed) player_at.y += player_speed * elapsed;

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	ppu.background_position = player_at;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---
	// Test loading tilemap
	

	//--- actually draw ---
	ppu.draw(drawable_size);
}
