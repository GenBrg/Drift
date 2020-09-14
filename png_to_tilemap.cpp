#include "load_save_png.hpp"
#include "read_write_chunk.hpp"
#include "PPU466.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <png file name without suffix>" << std::endl;
        return -1;
    }

    std::vector< glm::u8vec4 > data;
    glm::uvec2 size;
    std::string png_file_name(argv[1]);
    png_file_name += ".png";

    std::cout << "Processing " << png_file_name << std::endl;

    try {
        load_png(png_file_name, &size, &data, UpperLeftOrigin);
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    std::string tilemap_file_name(argv[1]);
    tilemap_file_name += ".tile";
    std::ofstream tilemap_file(tilemap_file_name, std::ios::binary);
    if (!tilemap_file.is_open()) {
        std::cerr << "Can't write to " << tilemap_file_name << std::endl;
    }

    std::string palette_file_name(argv[1]);
    palette_file_name += ".palette";
    std::ofstream palette_file(palette_file_name, std::ios::binary);
    if (!palette_file.is_open()) {
        std::cerr << "Can't write to " << palette_file_name << std::endl;
    }

    std::vector<PPU466::Palette> palette_table;
    std::vector<PPU466::Tile> tile_table(PPU466::TileTableWidth * PPU466::TileTableHeight, PPU466::Tile());

    // The first half in the first tile in the png is used to define palettes
    for (int i = 0; i < PPU466::PaletteTableNum; ++i) {
        PPU466::Palette palette;

        for (int j = 0; j < PPU466::PaletteColorNum; ++j) {
            int idx = i * PPU466::PaletteColorNum + j;
            const glm::u8vec4& color = data[(idx / PPU466::TileWidth) * PPU466::TileTableWidth * PPU466::TileWidth + idx % PPU466::TileHeight];
            palette[j] = color;
        }

        palette_table.push_back(std::move(palette));
    }

    PPU466::DebugPrintPaletteTable(palette_table.data()->data());

    // Process each tile
    for (int i = 0; i < PPU466::TileTableHeight; ++i) {
        for (int j = 0; j < PPU466::TileTableWidth; ++j) {
            if (i == 0 && j == 0) {
                continue;
            }

            PPU466::Tile& tile = tile_table[i * PPU466::TileTableWidth + j];
            std::vector<glm::u8vec4> temp_palette {};

            // Gather all colors in the tile
            for (size_t row = i * PPU466::TileHeight; row < (i + 1) * PPU466::TileHeight; ++row) {
                for (size_t col = j * PPU466::TileWidth; col < (j + 1) * PPU466::TileWidth; ++col) {
                    const glm::u8vec4& color = data[row * PPU466::TileWidth * PPU466::TileTableWidth + col];
                    if (std::find(temp_palette.begin(), temp_palette.end(), color) == temp_palette.end()) {
                        temp_palette.push_back(color);
                        if (temp_palette.size() > 4) {
                            std::cerr << "More than 4 colors appear in tile map (" << i << ", " << j << ")" << std::endl;
                            return -1;
                        }
                    }
                }
            }

            // Find the correct palette in palette_table
            int palette_idx = -1;
            size_t palette_num = palette_table.size();

            for (int i = 0; i < palette_num; ++i) {
                const PPU466::Palette& palette = palette_table[i];
                bool palette_match = true;

                for (const glm::u8vec4& color : temp_palette) {
                    if (std::find(palette.begin(), palette.end(), color) == palette.end()) {
                        std::cout << "Not found" << std::endl;
                        std::cout << " " << static_cast<int>(color.x) 
                                    << " " << static_cast<int>(color.y) 
                                    << " " << static_cast<int>(color.z) 
                                    << " " << static_cast<int>(color.w) 
                                    << std::endl;
                        std::cout << "In: " << std::endl;
                        PPU466::DebugPrintPalette(palette.data());
                        palette_match = false;
                        break;
                    }
                }

                if (palette_match) {
                    palette_idx = i;
                    break;
                }
            }

            if (palette_idx < 0) {
                PPU466::DebugPrintPalette(temp_palette.data());
                std::cerr << "Cannot find the palette for tile map (" << i << ", " << j << ")" << std::endl;
                return -1;
            }

            const PPU466::Palette& palette = palette_table[palette_idx];

            // Fill in the tile_table
            for (int y = 0; y < PPU466::TileHeight; ++y) {
                for (int x = 0; x < PPU466::TileWidth; ++x) {
                    int row = i * PPU466::TileHeight + 7 - y;
                    int col = j * PPU466::TileWidth + x;
                    const glm::u8vec4& color = data[row * PPU466::TileWidth * PPU466::TileTableWidth + col];
                    uint8_t color_idx = static_cast<uint8_t>(std::distance(palette.begin(), std::find(palette.begin(), palette.end(), color)));
                    PPU466::SetTilePixel(tile, x, y, color_idx);
                }
            }
        }
    }

    PPU466::DebugPrintTileMap(tile_table.data(), 10);

    write_chunk("pale", palette_table, &palette_file);
    write_chunk("tile", tile_table, &tilemap_file);

    return 0;
}
