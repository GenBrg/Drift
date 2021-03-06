#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {
	std::random_device r;
	mt.seed(r());

	ppu.background_color = glm::u8vec4(0x2F, 0x1C, 0xEA, 0xFF);

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

	// player sprite
	ppu.sprites[0].index = 11;
	ppu.sprites[0].attributes = PPU466::MakeSpriteAttributes(0, 3);

	// lives sprite
	ppu.sprites[1].index = 26;
	ppu.sprites[1].attributes = PPU466::MakeSpriteAttributes(0, 2);
	ppu.sprites[1].x = 32 * 8 - 4 * 8;
	ppu.sprites[1].y = 241;
	ppu.sprites[2].index = 26;
	ppu.sprites[2].attributes = PPU466::MakeSpriteAttributes(0, 2);
	ppu.sprites[2].x = 32 * 8 - 5 * 8;
	ppu.sprites[2].y = 241;
	ppu.sprites[3].index = 26;
	ppu.sprites[3].attributes = PPU466::MakeSpriteAttributes(0, 2);
	ppu.sprites[3].x = 32 * 8 - 6 * 8;
	ppu.sprites[3].y = 241;
	ppu.sprites[4].index = 26;
	ppu.sprites[4].attributes = PPU466::MakeSpriteAttributes(0, 2);
	ppu.sprites[4].x = 32 * 8 - 7 * 8;
	ppu.sprites[4].y = 241;
	ppu.sprites[5].index = 26;
	ppu.sprites[5].attributes = PPU466::MakeSpriteAttributes(0, 2);
	ppu.sprites[5].x = 32 * 8 - 8 * 8;
	ppu.sprites[5].y = 241;

	// score sprite
	ppu.sprites[6].index = 1;
	ppu.sprites[6].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[6].x = 24;
	ppu.sprites[6].y = 29 * 8;

	ppu.sprites[7].index = 1;
	ppu.sprites[7].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[7].x = 24 - 1 * 4;
	ppu.sprites[7].y = 29 * 8;

	ppu.sprites[8].index = 1;
	ppu.sprites[8].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[8].x = 24 - 2 * 4;
	ppu.sprites[8].y = 29 * 8;

	ppu.sprites[9].index = 1;
	ppu.sprites[9].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[9].x = 24 - 3 * 4;
	ppu.sprites[9].y = 29 * 8;

	ppu.sprites[10].index = 1;
	ppu.sprites[10].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[10].x = 24 - 4 * 4;
	ppu.sprites[10].y = 29 * 8;


	// highest score sprite
	ppu.sprites[11].index = 1;
	ppu.sprites[11].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[11].x = 128;
	ppu.sprites[11].y = 29 * 8;

	ppu.sprites[12].index = 1;
	ppu.sprites[12].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[12].x = 128 - 1 * 4;
	ppu.sprites[12].y = 29 * 8;

	ppu.sprites[13].index = 1;
	ppu.sprites[13].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[13].x = 128 - 2 * 4;
	ppu.sprites[13].y = 29 * 8;

	ppu.sprites[14].index = 1;
	ppu.sprites[14].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[14].x = 128 - 3 * 4;
	ppu.sprites[14].y = 29 * 8;

	ppu.sprites[15].index = 1;
	ppu.sprites[15].attributes = PPU466::MakeSpriteAttributes(0, 0);
	ppu.sprites[15].x = 128 - 4 * 4;
	ppu.sprites[15].y = 29 * 8;
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

	static float ocean_movement_y = 0;
	ocean_movement_y += ocean_speed * elapsed;
	int rows_to_update = static_cast<int32_t>(ocean_movement_y) / 8;
	score += rows_to_update;

	ocean_movement_y -= rows_to_update * 8.0f;
	while (rows_to_update--) {
		game_map.pop_front();
	}
	GenerateNewRows();

	camera_y_pos = 15 * 8.0f + ocean_movement_y;
	player_at.y = std::max(0.0f, player_at.y - ocean_speed * elapsed);
	
	ppu.background_position.y = -static_cast<int32_t>(camera_y_pos);

	// update ocean and items (scroll)
	player_at.x = std::min(64 * 8.0f, std::max(0.0f, player_at.x));
	player_at.y = std::min(60 * 8.0f, std::max(0.0f, player_at.y));

	// collision detection
	int occupied_grid_x = static_cast<int32_t>(player_at.x / 8.0f);
	int occupied_grid_y = static_cast<int32_t>((player_at.y + camera_y_pos) / 8.0f);

	ppu.sprites[0].attributes = PPU466::MakeSpriteAttributes(0, 3);
	CollideWith(occupied_grid_x, occupied_grid_y);
	CollideWith(occupied_grid_x + 1, occupied_grid_y);
	CollideWith(occupied_grid_x, occupied_grid_y + 1);
	CollideWith(occupied_grid_x + 1, occupied_grid_y + 1);

	// apply item effects
	ApplyEffects(elapsed);

	// draw overlay
	PrintScore(11, score);
	PrintScore(6, highest_score);

	// draw lives
	for (int i = 0; i < life; ++i) {
		ppu.sprites[1 + i].y = 27 * 8;
	}

	for (int i = life; i < max_life; ++i) {
		ppu.sprites[1 + i].y = 241;
	}

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
	#ifdef _DEBUG
	for (int i = 59; i >= 0; --i) {
		for (int j = 0; j < 32; ++j) {
			uint16_t tile = DrawTile(game_map[i][j]);
			std::cout << static_cast<int32_t>(tile) << " ";
		}
		std::cout << std::endl;
	}
	#endif
}

void PlayMode::CollideWith(int x, int y) {
	using Type = BackgroundTile::Type;
	if (x < 0 || x > 31 || y < 0 || y > 59)
		return;

	switch (game_map[y][x].type) {
		case Type::ITEM:
			if (RectIntersectTest(player_at.x, player_at.y, 4.0f, 4.0f, x * 8.0f, y * 8.0f - camera_y_pos, 4.0f, 4.0f)) {
				GetItem(static_cast<BackgroundTile::Item>(game_map[y][x].subid));
				static std::uniform_int_distribution<int> ocean_dist(0, 3);
				game_map[y][x] = { Type::OCEAN,  static_cast<uint8_t>(ocean_dist(mt)) };
			}
			break;
		case Type::ROCK:
			if (RectIntersectTest(player_at.x, player_at.y, 4.0f, 4.0f, x * 8.0f, y * 8.0f - camera_y_pos, 4.0f, 4.0f)) {
				if (invulnerable_time == 0.0f) {
					invulnerable_time = 3.0f;
					--life;
					if (life == 0) {
						life = max_life;
						highest_score = std::max(highest_score, score);
						score = 0;
					}
				}
			}
		break;
		default:;
	}
}

void PlayMode::GetItem(BackgroundTile::Item item) {
	using Item = BackgroundTile::Item;

	switch (item) {
		case Item::FAST_OCEAN:
			score += 5;
			ocean_speed = 20.0f;
			fast_ocean_time = 5.0f;
			slow_ocean_time = 0.0f;
		break;
		case Item::FAST_RAFT:
			score++;
			player_speed = 45.0f;
			fast_raft_time = 5.0f;
			slow_raft_time = 0.0f;
		break;
		case Item::SLOW_OCEAN:
			score++;
			ocean_speed = 5.0f;
			slow_ocean_time = 5.0f;
			fast_ocean_time = 0.0f;
		break;
		case Item::SLOW_RAFT:
			score += 5;
			player_speed = 20.0f;
			slow_raft_time = 5.0f;
			fast_raft_time = 0.0f;
		break;
		case Item::LIFE:
			score += 10;
			life = std::min(max_life, static_cast<uint8_t>(life + 1));
		break;
		case Item::INVULNERABLE_POTION:
			score += 3;
			invulnerable_time = 5.0f;
		break;
		default:;
	}

	
}

void PlayMode::ApplyEffects(float elapsed_time) {
	if (fast_ocean_time > 0.0f) {
		fast_ocean_time = std::max(0.0f, fast_ocean_time - elapsed_time);
		if (fast_ocean_time == 0.0f) {
			ocean_speed = 10.0f;
		}
	} else if (slow_ocean_time > 0.0f) {
		slow_ocean_time = std::max(0.0f, slow_ocean_time - elapsed_time);
		if (slow_ocean_time == 0.0f) {
			ocean_speed = 10.0f;
		}
	}

	if (fast_raft_time > 0.0f) {
		fast_raft_time = std::max(0.0f, fast_raft_time - elapsed_time);
		if (fast_raft_time == 0.0f) {
			player_speed = 30.0f;
		}
	} else if (slow_raft_time > 0.0f) {
		slow_raft_time = std::max(0.0f, slow_raft_time - elapsed_time);
		if (slow_raft_time == 0.0f) {
			player_speed = 30.0f;
		}
	}

	if (invulnerable_time > 0.0f) {
		ppu.sprites[0].attributes = PPU466::MakeSpriteAttributes(0, 6);
		invulnerable_time = std::max(0.0f, invulnerable_time - elapsed_time);
		if (invulnerable_time == 0.0f) {
			ppu.sprites[0].attributes = PPU466::MakeSpriteAttributes(0, 3);
		}
	}
}

void PlayMode::PrintScore(int start_sprite_idx, uint16_t score) {
	int i = 0;
	while (score > 0) {
		int digit = score % 10;
		ppu.sprites[start_sprite_idx + i].index = 1 + digit;
		++i;
		score /= 10;
	}

	while (i < 5) {
		ppu.sprites[start_sprite_idx + i].index = 1;
		++i;
	}
}
