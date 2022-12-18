#include "Common.hpp"
#include "Math"
#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <unordered_set>

namespace std {
	template <> struct hash<Vector2s> {
		unsigned long long operator()(const Vector2s& vec) const {
			return unsigned long long(vec.x) | (unsigned long long(vec.y) << 16ull);
		}
	};
}

namespace std {
	template <> struct hash<Vector2i> {
		unsigned long long operator()(const Vector2i& vec) const {
			return unsigned long long(vec.x) | (unsigned long long(vec.y) << 32ull);
		}
	};
}

int Day14()
{
	// same code will run much faster with unordered_set but for visualization I've used unordered_map here 
	std::unordered_map<Vector2s, char> world;
	short minX = SHRT_MAX, maxX = SHRT_MIN, maxY = 0;

	auto const UpdateBounds = [&](Vector2s current) {
		maxY = Max(current.y, maxY);
		minX = Min(current.x, minX);
		maxX = Max(current.x, maxX);
	};

	auto const Visualize = [&](int yStart, int yEnd)
	{
		system("cls");
		for (short i = yStart; i < yEnd +1; ++i) {
			for (short j = minX; j <= maxX; ++j) 
			{
				Vector2s vec = Vector2s(j, i);
				if (vec == Vector2s(500, 0)) printf("%c", 'X');
				else if (world.find(vec) != world.end())
					printf("%c", world[vec]);
				else printf("%c", '.');
			}
			printf("\n");
		}	
	};

	auto const FindNext = [&](Vector2s position)
	{
		if (position.y == maxY) return Vector2s::Zero();
		if (world.end() == world.find(position + Vector2s::Up()))  return position + Vector2s::Up();
		if (world.end() == world.find(position + Vector2s(-1, 1))) return position + Vector2s(-1, 1);
		if (world.end() == world.find(position + Vector2s( 1, 1))) return position + Vector2s( 1, 1);
		return Vector2s::Zero();
	};
	
	{
		FILE* file = fopen("Assets/AOC14.txt", "r");
		char line[300];

		while (fgets(line, sizeof(line), file))
		{
			const char* curr = line;
			Vector2s current{};
			ParseNumberRef<short>(curr, current.x);
			ParseNumberRef<short>(++curr, current.y);

			while (!IsNumber(*curr)) curr++;

			UpdateBounds(current);
			int axis = 0;

			while (*curr > '\n' && IsNumber(*curr))
			{
				world[current] = '#';
				short target = ParseNumber<short>(curr);
				short direction = target - current[axis] > 0 ? 1 : -1;
				while (abs(current[axis] - target) > 0) {
					current[axis] += direction;
					world[current] = '#';
				}
				UpdateBounds(current);
				axis ^= 1;
				while (*curr > '\n' && !IsNumber(*curr)) curr++;
			}
		}
		fclose(file);	
	}

	maxY++;
	int numSand = 0;
	short testMax = 0;

	while (true)
	{
		Vector2s current(500, 0);
		Vector2s next;

		// if (world.find(current) != world.end()) break; // open for part2

		while ((next = FindNext(current)) != Vector2s::Zero())
		{
			current = next;
		}
		world[current] = 'o';
		testMax = Max(testMax, current.y);
		maxX = Max(current.x, maxX);
		minX = Min(current.x, minX);
		// Visualize(Max(testMax-50, 0), testMax+1);
		// using namespace std::chrono_literals;
        // std::this_thread::sleep_for(500ms);  // requires std::chrono
		if (current.y >= maxY) break; // close for part2
		numSand++;
	}
	// Visualize(0, maxY+1);
	printf("num sands: %d", numSand);
	return numSand;
}

template<typename T>
inline int ManhattanDistance(Vector2<T> a, Vector2<T> b)
{
	return abs(a.x - b.x) + abs(a.y - b.y);
}

int Day15()
{
	FILE* file = fopen("Assets/AOC15.txt", "r");
	
	char line[120];
	std::unordered_map<Vector2i, int> sensors;
	std::unordered_set<int> beaconYs;
	Vector2i boundsMin = INT_MAX, boundsMax = INT_MIN;

	while (fgets(line, sizeof(line), file))
	{
		const char* curr = line;
		
		Vector2i sensorPos = ParseVector<Vector2i>(curr);
		Vector2i beaconPos = ParseVector<Vector2i>(curr);
		Vector2i distance  = Vector2i(abs(sensorPos.x - beaconPos.x), abs(sensorPos.y - beaconPos.y)); // ManhattanDistance
		sensors[sensorPos] = distance.x + distance.y;
		beaconYs.insert(beaconPos.y);
		boundsMin = MinT(boundsMin, beaconPos - distance);
		boundsMax = MaxT(boundsMax, beaconPos + distance);
	}

	int result = 0;
	// check each column if it contains # or not
	for (int j = boundsMin.x; j <= boundsMax.x; ++j) 
	{
		Vector2i columnPos = Vector2i(j, 2'000'000);
		if (beaconYs.contains(columnPos.y)) continue;
		for (auto const& [pos, dist] : sensors)
		{
			int columnToSensor = ManhattanDistance(pos, columnPos) 
			if (columnToSensor > 0 && columnToSensor <= dist) { result++; break; }
		}
	}
	printf("result: %d", result);
	return 0;
}

int Day17()
{
	char* pattern = ReadAllFile("Assets/AOC17.txt");
	
	const Vector2s shapes[5][5] = {
		{ Vector2s(0, 0), Vector2s(1, 0), Vector2s(2, 0), Vector2s(3, 0) }, // ####
		{ Vector2s(1, 0), Vector2s(0, 1), Vector2s(1, 1), Vector2s(2, 1), Vector2s(1, 2) }, // + shape
		{ Vector2s(0, 0), Vector2s(1, 0), Vector2s(2, 0), Vector2s(2, 1), Vector2s(2, 2) }, // L shape
		{ Vector2s(0, 0), Vector2s(0, 1), Vector2s(0, 2), Vector2s(0, 3) }, // | shape
		{ Vector2s(0, 0), Vector2s(1, 0), Vector2s(0, 1), Vector2s(1, 1) } // box shape
	};
	const int shapeSizes[5] = { 4,5,5,4,4 }; // num pixels

	Vector2s mapBounds = Vector2s(7, -1);
	std::unordered_set<Vector2s> blocks;

	auto const CheckColission = [&](const Vector2s* shape, int shapeSize, Vector2s position) 
	{
		bool collided = false;
		for (int i = 0; i < shapeSize; ++i)
		{
			Vector2s pixel = shape[i] + position;
			collided |= blocks.contains(pixel) | pixel.x >= 7 | pixel.x < 0 | pixel.y < 0;
		}
		return collided;
	};

	const char* curr = pattern;
	int currBlock = 0, cleanHeight = 200;

	for (int g = 0; g < 2022; ++g)
	{
		const int blockIndex = currBlock++ % 5;
		const int shapeSize = shapeSizes[blockIndex];
		const Vector2s* shape = shapes[blockIndex];
		Vector2s position = Vector2s(2, mapBounds.y + 4);

		auto const visualize = [&]()
		{
			for (int i = 0; i < shapeSize; ++i) 
				blocks.insert(position + shape[i]);
		
			system("cls");
			for (int i = Max<short>(mapBounds.y, 7)+4; i >= 0; --i)
			{
				for (int j = 0; j < mapBounds.x; ++j)
				{
					if (blocks.contains(Vector2s(j, i))) printf("#");
					else printf(".");
				}
				printf("\n");
			}
		
			for (int i = 0; i < shapeSize; ++i) 
				blocks.erase(position + shape[i]);
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(300ms);
		};

		//visualize();
		while (true)
		{
			short direction = *curr++ - '='; // with ascii table it is clear to understand: '<','=','>'  returns -1 or 1 depending on input axis 
			position.x += direction;
			if (CheckColission(shape, shapeSize, position)) position.x -= direction;
			// 	visualize();
			position.y -= 1; // gravity
			if (CheckColission(shape, shapeSize, position)) { position.y++; break; }
			// visualize();
			if (*curr <= '\n')  curr = pattern; // if we are in the end of the pattern repeat pattern
		}

		for (int i = 0; i < shapeSizes[blockIndex]; ++i)
		{
			Vector2s pixel = shape[i] + position;
			blocks.insert(pixel); // insert pixels
			mapBounds.y = Max(mapBounds.y, pixel.y); // check new pixel is higher than top point
		}
		// we don't want to store all of the data that's why we are removing unnecesarry blocks that too below us
		if (mapBounds.y > cleanHeight + 100)
		{
			for (auto it = std::begin(blocks); it != std::end(blocks);)
			{
				if (it->y < cleanHeight)
					it = blocks.erase(it);
				else
					++it;
			}
			cleanHeight += 100;
		}
	}

	// writing to a file is aditional, not must
	FILE* file = fopen("Assets/17Result.txt", "w");	
	for (short i = Max<short>(mapBounds.y, 7) + 4; i >= 0; --i)
	{
		char line[8];
		for (short j = 0; j < 7; ++j)
		{
			if (blocks.contains(Vector2s(j, i))) line[j] = '#';
			else line[j] = '.';
		}
		line[7] = '\n'; fwrite(line, 1, 8, file);
	}
	fclose(file);
	free(pattern);
	printf("result is: %d", mapBounds.y+1);
	return mapBounds.y;
}
