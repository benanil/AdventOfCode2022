#include "Common.hpp"
#include "Math"
#include <stdio.h>
#include <string.h>
#include <unordered_map>

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
		Vector2i distance = ManhattanDistance(sensorPos, beaconPos);
		sensors[sensorPos] = distance.x;
		beaconYs.insert(beaconPos.y);
		boundsMin = MinT(boundsMin, beaconPos - distance);
		boundsMax = MaxT(boundsMax, beaconPos + distance);
	}

	int result = 0;
	for (int j = boundsMin.x; j <= boundsMax.x; ++j)
	{
		Vector2i position = Vector2i(j, 2'000'000);
		
		for (auto const& [pos, dist] : sensors)
		{
			if (ManhattanDistance(pos, position) <= dist) { result++; break; }
		}
	}

	for (auto const& y: beaconYs) 
	{
		if (y == 2'000'000) { result--; }
	}

	printf("result: %d", result);
	return 0;
}
