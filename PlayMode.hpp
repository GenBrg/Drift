#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <random>

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

	//player position:
	glm::vec2 player_at = glm::vec2(0.0f);
	float camera_y_pos = 15 * 8.0f;

	float player_speed { 30.0f };
	float ocean_speed { 15.0f };

	struct BackgroundTile {
		enum class Type : uint8_t {
			OCEAN,
			ROCK,
			ITEM
		};

		enum class Item : uint8_t {
			FAST_RAFT = 0,
			SLOW_RAFT,
			FAST_OCEAN,
			SLOW_OCEAN,
			LIFE,
			INVULNERABLE_POTION
		};

		Type type;
		uint8_t subid;
	};

	float fast_raft_time { 0.0f };
	float slow_raft_time { 0.0f };
	float fast_ocean_time { 0.0f };
	float slow_ocean_time { 0.0f };
	float invulnerable_time { 0.0f };

	uint32_t score { 0 };
	uint8_t life { 3 };

	std::deque<std::vector<BackgroundTile>> game_map;

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
	std::mt19937 mt;

	BackgroundTile GenerateNewTile();
	void GenerateNewRows();
	uint16_t DrawTile(const BackgroundTile& tile);
	void DebugPrintMap();
};
