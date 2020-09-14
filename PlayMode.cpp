#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {
	std::random_device r;
	mt.seed(r());

	ppu.background_color = glm::u8vec4(0x00, 0x00, 0x00, 0xff);

	GenerateNewRows();

	// place player
	int lower_left_tile_y = static_cast<int>(camera_y_pos) / 8 + 1;
	bool placed = false;
	for (int i = lower_left_tile_y; i < std::min(lower_left_tile_y + 31, 60) && !placed; ++i) {
		for (int j = 0; j < 32; ++j) {
			if (game_map[i][j].type == BackgroundTile::Type::OCEAN) {
				player_at.x = j * 8.0f;
				player_at.y = i * 8.0f - camera_y_pos;

				placed = true;
				break;
			}
		}
	}

	ppu.background_position.x = 0;
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

	ppu.background_position.y = -static_cast<int32_t>(camera_y_pos);

	// update ocean and items (scroll)
	player_at.x = std::min(64 * 8.0f, std::max(0.0f, player_at.x));
	player_at.y = std::min(60 * 8.0f, std::max(0.0f, player_at.y));

	// collision detection

	// apply item effects

	// draw overlay

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---
	// Draw player
	ppu.sprites[0].x = static_cast<uint8_t>(player_at.x);
	ppu.sprites[0].y = static_cast<uint8_t>(player_at.y);
	ppu.sprites[0].index = 11;
	ppu.sprites[0].attributes = PPU466::MakeSpriteAttributes(0, 3);

	// Upload game_map within the viewport to ppu
	int lower_left_tile_y = static_cast<int32_t>(camera_y_pos) / 8;
	for (int i = lower_left_tile_y; i < std::min(lower_left_tile_y + 31, 60); ++i) {
		for (int j = 0; j < 32; ++j) {
			ppu.background[i * PPU466::BackgroundWidth + j] = DrawTile(game_map[i][j]);
		}
	}
	
	//--- actually draw ---
	ppu.draw(drawable_size);
}

PlayMode::BackgroundTile PlayMode::GenerateNewTile() {
	using Type = PlayMode::BackgroundTile::Type;
	using Item = PlayMode::BackgroundTile::Item;
	static std::uniform_int_distribution<int> dist(1, 10000);
	int dice = dist(mt);
	Type type;
	uint8_t subid;

	// 0.5% item: 6% life, 14% invulnerable potion, 20% speed control
	// 69.5% ocean
	// 30% rock

	if (dice <= 50) {
		type = Type::ITEM;
		if (dice <= 3) {
			subid = static_cast<uint8_t>(Item::LIFE);
		} else if (dice <= 10) {
			subid = static_cast<uint8_t>(Item::INVULNERABLE_POTION);
		} else {
			subid = (dice - 11) / 10;
		}
	} else if (dice <= 7000) {
		type = Type::OCEAN;
		subid = (dice - 51) / 1737;
	} else {
		type = Type::ROCK;
		subid = (dice - 7001) / 750;
	}

	return { type, subid };
}

void PlayMode::GenerateNewRows() {
	using Type = PlayMode::BackgroundTile::Type;
	assert(game_map.size() <= PPU466::BackgroundHeight);

	size_t rows_to_generate = PPU466::BackgroundHeight - game_map.size();

	while (rows_to_generate--) {
		std::vector<PlayMode::BackgroundTile> new_row(32);
		static std::uniform_int_distribution<int> dist(0, 3);

		new_row[0] = { Type::ROCK, static_cast<uint8_t>(dist(mt)) };
		new_row[31] = { Type::ROCK, static_cast<uint8_t>(dist(mt)) };

		for (int i = 1; i < 31; ++i) {
			new_row[i] = GenerateNewTile();
		}

		game_map.push_back(std::move(new_row));
	}
}

uint16_t PlayMode::DrawTile(const BackgroundTile& tile) {
	using Type = BackgroundTile::Type;
	using Item = BackgroundTile::Item;

	uint8_t tile_index = 0;
	uint8_t palette_index = 0;

	switch (tile.type) {
		case Type::ITEM:
			switch (static_cast<Item>(tile.subid))
			{
			case Item::FAST_OCEAN:
				tile_index = 18;
				palette_index = 2;
				break;
			case Item::SLOW_OCEAN:
				tile_index = 19;
				palette_index = 4;
				break;
			case Item::FAST_RAFT:
				tile_index = 16;
				palette_index = 2;
				break;
			case Item::SLOW_RAFT:
				tile_index = 17;
				palette_index = 4;
				break;
			case Item::LIFE:
				tile_index = 20;
				palette_index = 2;
				break;
			case Item::INVULNERABLE_POTION:
				tile_index = 21;
				palette_index = 2;
				break;
			default:
				throw std::runtime_error("Unexpected item");
			}
		break;
		case Type::OCEAN:
		tile_index = 12 + tile.subid;
		palette_index = 1;
		break;
		case Type::ROCK:
		tile_index = 22 + tile.subid;
		palette_index = 5;
		break;
		default:
			throw std::runtime_error("Unexpected tile type");
	}

	return PPU466::MakeBackgroundValue(tile_index, palette_index);
}

void PlayMode::DebugPrintMap() {
	for (int i = 59; i >= 0; --i) {
		for (int j = 0; j < 32; ++j) {
			uint16_t tile = DrawTile(game_map[i][j]);
			std::cout << static_cast<int32_t>(tile) << " ";
		}
		std::cout << std::endl;
	}
}
