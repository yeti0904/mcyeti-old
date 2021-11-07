#pragma once
#include <cstdint>
#include <cmath>
#include <cstring>
#include <vector>
#include <unordered_map>
#include "player.hh"
using std::vector;
using std::unordered_map;

class level {
	public:
	level() {}
	// variables
	vec3 size;
	uint8_t* raw;
	uint8_t perbuild, pervisit;
	unordered_map <uint16_t, unordered_map <uint16_t, unordered_map <uint16_t, uint8_t>>> map;

	// functions
	void generate(int16_t x, int16_t y, int16_t z) {
		size.x = x;
		size.y = y;
		size.z = z;
		for (int16_t i = 0; i<z; ++i) {
			for (int16_t j = 0; j<(int)round(y/2); ++j) {
				for (int16_t k = 0; k<y; ++k) {
					map[i][j][k] = 1;
				}
			}
		}
	}
	uint8_t* getraw() {
		uint32_t blockAmount = size.x * size.y * size.y;
		uint8_t  blockAmountBytes[4];
		vector <uint8_t> data;
		uint8_t* ret;

		// convert int32 to byte array
		memcpy(blockAmountBytes, &blockAmount, sizeof(blockAmount));
		
		// push block amount to data
		data = {
			blockAmountBytes[0],
			blockAmountBytes[1],
			blockAmountBytes[2],
			blockAmountBytes[3]
		};

		// add rest of block data to array
		for (int16_t z = 0; z<size.z; ++z) {
			for (int16_t y = 0; y<size.y; ++y) {
				for (int16_t x = 0; x<size.x; ++x) {
					data.push_back(map[x][y][z]);
				}
			}
		}

		// return raw vector data
		return data.data();
	}
};