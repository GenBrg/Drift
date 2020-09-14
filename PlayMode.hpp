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
	float ocean_speed { 10.0f };

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

	uint16_t score { 0 };
	uint16_t highest_score { 0 };
	inline static constexpr uint8_t max_life { 5 };
	uint8_t life { max_life };

	std::deque<std::vector<BackgroundTile>> game_map;

	//----- drawing handled by PPU466 -----

	PPU466 ppu;
	std::mt19937 mt;

	BackgroundTile GenerateNewTile();
	void GenerateNewRows();
	uint16_t DrawTile(const BackgroundTile& tile);
	void DebugPrintMap();
	bool SegmentIntersectTest(float x1, float x2, float y1, float y2) {
		return (x2 > y1) && (x1 < y2);
	}
	bool RectIntersectTest(float x1, float y1, float width1, float height1,
							 float x2, float y2, float width2, float height2) {
		return SegmentIntersectTest(x1, x1 + width1, x2, x2 + width2) &&
				SegmentIntersectTest(y1, y1 + height1, y2, y2 + height2);
	}
	void CollideWith(int x, int y);
	void GetItem(BackgroundTile::Item item);
	void ApplyEffects(float elapsed_time);
	void PrintScore(int start_sprite_idx, uint16_t score);
};
