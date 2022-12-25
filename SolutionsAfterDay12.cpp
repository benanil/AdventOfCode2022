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

int main()
{
	FILE* file = fopen("Assets/AOC15.txt", "r");

	char line[120];
	std::unordered_map<Vector2i, int> sensors;
	std::unordered_set<int> beaconXs;
	Vector2i boundsMin = INT_MAX, boundsMax = INT_MIN;

	while (fgets(line, sizeof(line), file))
	{
		const char* curr = line;

		Vector2i sensorPos = ParseVector<Vector2i>(curr);
		Vector2i beaconPos = ParseVector<Vector2i>(curr);
		Vector2i distance = Vector2i(abs(sensorPos.x - beaconPos.x), abs(sensorPos.y - beaconPos.y)); // ManhattanDistance
		sensors[sensorPos] = distance.x + distance.y;
		beaconXs.insert(beaconPos.y == 2'000'000 ? beaconPos.x : INT_MIN);
		boundsMin = MinT(boundsMin, beaconPos - distance);
		boundsMax = MaxT(boundsMax, beaconPos + distance);
	}

	int result = 0;
	// check each column if it contains # or not
	for (int j = boundsMin.x; j <= boundsMax.x; ++j)
	{
		Vector2i columnPos = Vector2i(j, 2'000'000);
		if (sensors.contains(columnPos)
			|| beaconXs.contains(columnPos.x)) continue; // if this beacon is sensor we will not count this
		
		for (auto const& [pos, dist] : sensors)
		{
			int columnToSensor = ManhattanDistance(pos, columnPos);
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

#include <fstream>
#include <string>

int Day18()
{
	bool space[22][22][22] = {0};
	std::vector<Vector3c> vectors;
	// for some reason c file system didn't worked well for this file
	std::ifstream file{ "Assets/AOC18.txt" , std::ios::in};
	std::string line;

	// Read each line from the file
	while (std::getline(file, line))
	{
		const char* curr = &line[0];
		Vector3c vec = ParseVector<Vector3c>(curr);
		space[vec.x][vec.y][vec.z] = true;
		vectors.push_back(vec);
	}

	int totalSurface = vectors.size() * 6; // total surface area if we count all of the edges, but we will remove intersected ones

	for (Vector3c const& vec : vectors)
	{
		totalSurface -= space[vec.x+0][vec.y+1][vec.z+0];
		totalSurface -= space[vec.x+0][vec.y-1][vec.z+0];
		totalSurface -= space[vec.x+1][vec.y+0][vec.z+0];
		totalSurface -= space[vec.x-1][vec.y+0][vec.z+0];
		totalSurface -= space[vec.x+0][vec.y+0][vec.z+1];
		totalSurface -= space[vec.x+0][vec.y+0][vec.z-1];
	}
	printf("result is: %d", totalSurface);
	return 0;
}

int Day22()
{
	FILE* file = fopen("Assets/AOC22.txt", "r");
	char line[5666];

	std::unordered_map<Vector2i, char> map;
	Vector2i mapBounds = 0;
	// parse map
	while (fgets(line, sizeof(line), file))
	{
		if (IsNumber(*line)) break;
		const char* curr = line;
		int oldMaxX = mapBounds.x;
		mapBounds.x = 0;

		while (*curr > '\n')
		{
			if (!IsWhitespace(*curr)) map[mapBounds] = *curr;
			mapBounds.x++, curr++;
		}
		mapBounds.x = Max(mapBounds.x, oldMaxX);
		mapBounds.y++;
	}
	const char* path = line; // last line of text is our path

	const Vector2i directions[4] = {
		Vector2i( 0,-1),
		Vector2i( 1, 0), // we start from here, and depending on the input we go down or up, if we out of bounds we loop this array x % 4
		Vector2i( 0, 1),
		Vector2i(-1, 0)
	};

	int currentDirection = 1;
	Vector2i currentPosition(0, 0);

	auto const FindLoopPosition = [&](Vector2i position, const Vector2i& inverseDir)
	{
		while (map.contains(position + inverseDir))
		{
			position += inverseDir;
		}
		return map[position] == '#' ? -Vector2i::One() : position;
	};

	auto const Visualize = [&]()
	{
		system("cls"); // clear screen
		for (int y = Max(currentPosition.y-30, 0); y < Min(currentPosition.y+30, mapBounds.y); ++y)
		{
			for (int x = 0; x < mapBounds.x; ++x)
			{
				Vector2i pos = Vector2i(x, y);
				auto const find = map.find(pos);
				if (pos == currentPosition) printf("X");
				else if (find == map.end()) printf(" ");
				else printf("%c", find->second);
			}
			printf("\n");
		}
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(500ms);
	};

	// find startPosition
	while (!map.contains(currentPosition)) currentPosition.x++;

	while (*path > '\n')
	{
		int steps = ParseNumber<int>(path);

		while (steps--)
		{
			Vector2i direction = directions[currentDirection];
			Vector2i newPos = currentPosition + direction;
			const auto find = map.find(newPos);
			
			if (find == map.end())
			{
				// find loop map position by going inverse of our direction
				Vector2i loopPosition = FindLoopPosition(currentPosition, -direction);
				if (loopPosition == -Vector2i::One()) break;
				else { currentPosition = loopPosition; continue; }
			}

			if (find->second == '#') break;
			
			currentPosition = newPos;
			// Visualize();
		}

		int direction = *path++;
		if (direction == 'L') currentDirection = currentDirection == 0 ? 3 : currentDirection - 1;
		if (direction == 'R') currentDirection = currentDirection == 3 ? 0 : currentDirection + 1;
	}
	// rotate left for getting index
	currentDirection = currentDirection == 0 ? 3 : currentDirection - 1; // our indices start from 1 to 3

	currentPosition += Vector2i::One(); // result = one indexed array
	int result = 1000 * currentPosition.y + (4 * currentPosition.x) + currentDirection;

	printf("result: %d", result);
	return 0;
}

struct MapBitRow {
	union {
		struct { uint64 a, b, c; };
		uint64 bits[3];
	};
	MapBitRow(): a(0ull), b(0ull), c(0ull) {}
	
	bool Get(int idx) {
		return !!(bits[idx >> 6] & (1ull << uint64(idx & 63ull)));
	}
	void Set(int idx)   { bits[idx >> 6] |=   1ull << (idx & 63ull);  }
	void Reset(int idx) { bits[idx >> 6] &= ~(1ull << (idx & 63ull)); }
};

// you can use this struct in order to solve test case
struct MiniBitRow {
	uint32 bits;
	MiniBitRow() : bits(0u) {}

	bool Get(int idx) {
		return !!(bits & (1u << uint32(idx & 63u)));
	}
	void Set(int idx) { bits |= 1u << (idx & 63u); }
	void Reset(int idx) { bits &= ~(1u << (idx & 63u)); }
	void RotateRight() {
		constexpr unsigned int mask = 0b111111u;  // mask with 6 1's
		unsigned int rotated = (bits & mask) << 1u | (bits & mask) >> (6u - 1u);  // rotate the 5 bits
		bits = (bits & ~mask) | rotated;  // combine the rotated bits with the rest of the 
	}
	void RotateLeft() {
		unsigned int mask = 0b111111u;  // mask with 6 1's
		unsigned int rotated = (bits & mask) >> 1u | (bits & mask) << (6u - 1u);  // rotate the 6 bits
		bits = (bits & ~mask) | rotated;  // combine the rotated bits with the rest of the integer
	}
};

int Day24()
{
	char* mapText = ReadAllFile("Assets/AOC24.txt");
    // bitsets for determinating blizards
	MapBitRow rightRows[21] = {}; 
	MapBitRow leftRows [21] = {};
	MapBitRow upRows   [21] = {};
	MapBitRow downRows [21] = {};
	Vector2s mapBounds{};
	const char* currMap = mapText;
	// skip first line
	while (*currMap++ != '\n');

	while (currMap[1] != '#')
	{
		mapBounds.x = 0;
		currMap++;
		while (*currMap != '#')
		{
			if (*currMap == '>') rightRows[mapBounds.y].Set(mapBounds.x);
			if (*currMap == '<') leftRows [mapBounds.y].Set(mapBounds.x);
			if (*currMap == '^') upRows   [mapBounds.y].Set(mapBounds.x);
			if (*currMap == 'v') downRows [mapBounds.y].Set(mapBounds.x);
			currMap++;
			mapBounds.x++;
		}
		currMap += 2;
		mapBounds.y++;
	}
	CSTIMER("speed: ");

	std::unordered_set<Vector2s> players;
	players.insert(Vector2s(0,-1));
	players.insert(Vector2s(0, 0));
	
	const Vector2s endPos = mapBounds + Vector2s::Left();
	const Vector2s startPos = Vector2s(0, -1);

	auto const isBlocked = [mapBounds](Vector2s p) {
		return (p.x < 0 || p.y < 0 || p.x >= mapBounds.x || p.y >= mapBounds.y);
	};

	auto const isFreezed = [&](const Vector2s& pos)
	{
		if (pos == startPos) return false;
		return rightRows[pos.y].Get(pos.x) || leftRows[pos.y].Get(pos.x)
			|| upRows[pos.y].Get(pos.x) || downRows[pos.y].Get(pos.x);
	};

	auto const UpdateMap = [&]()
	{
		for (int i = 0; i < mapBounds.y; ++i) {
			// rotate right rows to right
			MapBitRow& rightRow = rightRows[i];
			bool lastWasSet = !!(rightRow.bits[0] & (1ull << 63));
			rightRow.bits[0] <<= 1;
			bool wasSet = !!(rightRow.bits[1] & (1ull << 63));
			rightRow.bits[1] <<= 1;
			rightRow.bits[1] |= lastWasSet;
			lastWasSet = !!(rightRow.bits[2] & (1ull << (21ull))); // 21 is for getting last bit on the bitset
			rightRow.bits[2] <<= 1;
			rightRow.bits[2] |= wasSet;
			rightRow.bits[0] |= lastWasSet;
		}
		for (int i = 0; i < mapBounds.y; ++i) {
			// rotate left rows to left
			MapBitRow& leftRow = leftRows[i];
			uint64 aSet = leftRow.bits[0] & 1ull;
			uint64 bSet = leftRow.bits[1] & 1ull;
			uint64 cSet = leftRow.bits[2] & 1ull;
			leftRow.bits[0] >>= 1;
			leftRow.bits[0] |= bSet * (1ull << 63);
			leftRow.bits[1] >>= 1;
			leftRow.bits[1] |= cSet * (1ull << 63);
			leftRow.bits[2] >>= 1;
			leftRow.bits[2] |= (1ull << 21ull) * aSet;
		}
		// rotate down rows
		MapBitRow tempRow = downRows[mapBounds.y - 1];
		for (int i = mapBounds.y - 1; i > 0; i--) {
			downRows[i] = downRows[i - 1];
		}
		downRows[0] = tempRow;
        // rotate up rows
		tempRow = upRows[0];
		for (int i = 0; i < mapBounds.y - 1; i++) {
			upRows[i] = upRows[i + 1];
		}
		upRows[mapBounds.y - 1] = tempRow;
	};

	auto const visualize = [&]()
	{
		system("cls");
		for (int j = -1; j <= mapBounds.x; ++j) 
			if (players.contains(Vector2s(j, -1))) printf("E"); 
			else if (j != 0) printf("#"); 
			else printf(".");
		printf("\n");

		for (int i = 0; i < mapBounds.y; ++i)
		{
			printf("#");
			for (int j = 0; j < mapBounds.x; ++j)
			{
				bool hasRight = rightRows[i].Get(j);
				bool hasLeft = leftRows[i].Get(j);
				bool hasUp = upRows[i].Get(j);
				bool hasDown = downRows[i].Get(j);
				int count = hasRight + hasLeft + hasUp + hasDown;

				Vector2s playerTest(j, i);
				if (players.contains(playerTest)) printf("E");
				else if (count == 0) printf(".");
				else if (count == 1) {
					if (hasRight) printf(">"); if (hasLeft)  printf("<"); if (hasUp) printf("^"); if (hasDown)  printf("v");
				}
				else {
					printf("%d", count);
				}
			}
			printf("#\n");
		}
		for (int j = -1; j <= mapBounds.x; ++j) if (j != mapBounds.x - 1) printf("#"); else printf(".");
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(200ms);
	};
	
	int minute = 0, end = 0;
	std::vector<Vector2s> added;

	while (!end)
	{
		minute++;
		UpdateMap();
		
		for (auto it = std::begin(players); it != std::end(players);)
		{
			if (isFreezed(*it))
				it = players.erase(it);
			else
				++it;
		}
		// visualize();
	    auto const proceedIfMoveable = [&](const Vector2s& pos)
		{
			if (pos == endPos) { end = 1; return; }
			if (pos == startPos) { added.push_back(pos); return; }
			if (!isBlocked(pos)) added.push_back(pos);
		};
		for (const Vector2s& p : players)
		{
			proceedIfMoveable(p + Vector2s::Down());
			proceedIfMoveable(p + Vector2s::Up());
			proceedIfMoveable(p + Vector2s::Left());
			proceedIfMoveable(p + Vector2s::Right());
		}
		while (!added.empty()) players.insert(added.back()), added.pop_back();
	}
	end_simulation:{}
	printf("minutes: %d", ++minute);
	free(mapText);
}
