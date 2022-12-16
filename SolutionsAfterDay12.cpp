#include "Common.hpp"
#include "Math"
#include <stdio.h>
#include <unordered_map>

std::unordered_map<Vector2s, char> world;
short minX = SHRT_MAX, maxX = SHRT_MIN, maxY = 0;

void UpdateBounds(Vector2s current) {
	maxY = Max(current.y, maxY);
	minX = Min(current.x, minX);
	maxX = Max(current.x, maxX);
}

void Visualize(int yStart, int yEnd)
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
}

Vector2s FindNext(Vector2s position)
{
	if (position.y == maxY) return Vector2s::Zero();
	if (world.end() == world.find(position + Vector2s::Up()))  return position + Vector2s::Up();
	if (world.end() == world.find(position + Vector2s(-1, 1))) return position + Vector2s(-1, 1);
	if (world.end() == world.find(position + Vector2s( 1, 1))) return position + Vector2s( 1, 1);
	return Vector2s::Zero();
}

int main()
{
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
	// Visualize(0, maxY);
	int numSand = 0;
	short testMax = 0;

	while (true)
	{
		Vector2s current(500, 0);
		Vector2s next;

		while ((next = FindNext(current)) != Vector2s::Zero())
		{
			current = next;
		}
		world[current] = 'o';
		testMax = Max(testMax, current.y);

		//Visualize(Max(testMax-50, 0), testMax+1);
		// using namespace std::chrono_literals;
        // std::this_thread::sleep_for(500ms); // requires std::chrono
		if (current.y >= maxY) break;
		numSand++;
	}
	printf("num sands: %d", numSand);
	return numSand;
}
