#include <fstream>
#include <filesystem>
#include <stdio.h>
#include <string.h>

template<typename T> inline T Max(const T a, const T b) { return a > b ? a : b; }
template<typename T> inline T Min(const T a, const T b) { return a < b ? a : b; }
template<typename T> inline T Clamp(const T x, const T a, const T b) { return Max(a, Min(b, x)); }
template<typename T> inline void Swap(T& a, T& b) { T t = a; a = b, b = t; }

typedef unsigned long long ulong;
typedef unsigned int uint;
typedef unsigned long long uint64;
typedef unsigned short ushort;

constexpr inline ulong MurmurHash(ulong h) {
    h ^= h >> 33ul;
    h *= 0xff51afd7ed558ccdUL;
    h ^= h >> 33ul;
    h *= 0xc4ceb9fe1a85ec53UL;
    h ^= h >> 33ul;
    return h;
}

constexpr inline uint WangHash(uint s) {
    s = (s ^ 61u) ^ (s >> 16u);
    s *= 9, s = s ^ (s >> 4u);
    s *= 0x27d4eb2du;
    s = s ^ (s >> 15u);
    return s;
}

constexpr inline uint StringToHash(const char* str, uint hash = 0)
{
    while (*str) // && !IsWhitespace(*str) && *str != '\n' // for more safety
        hash = *str++ + (hash << 6u) + (hash << 16u) - hash;
    return hash;
}

constexpr inline uint PathToHash(const char* str)
{
    uint hash = 0u, idx = 0u, shift = 0u;
    while (str[idx] && idx < 4u) // && *str != '\n' && !IsWhitespace(*str)
        hash |= uint(str[idx]) << shift, shift += 8u, idx++;
    return StringToHash(str + idx, WangHash(hash));
}

inline int ParseInt(const char*& curr)
{
    int instructionVal = 0;
    bool negative = false;
    if (*curr == '-') curr++, negative = true;
    while (!IsWhitespace(*curr) && *curr != '\n')
        instructionVal = instructionVal * 10 + (*curr++ - '0');
    return negative ? -instructionVal : instructionVal;
}

template<typename T> void FillN(T* arr, T val, int n) 
{
    for (int i = 0; i < n; ++i) arr[i] = val;
}

char* Helper_ReadAllText(const char* fileName, int* numCharacters = 0)
{
    FILE* fp = fopen(fileName, "r");

    if (!fp) {
        printf("file read failed");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    int fsize = ftell(fp);
    rewind(fp);
    // BOM check
    unsigned char test[3] = { 0 };
    fread(test, 1, 3, fp);

    if (test[0] != 0xEF || test[1] != 0xBB || test[2] != 0xBF) {
        rewind(fp);
    }

    char* fcontent = (char*)calloc(fsize + 1, 1);
    fread(fcontent, 1, fsize, fp);
    fclose(fp);
    if (numCharacters) *numCharacters = fsize;
    return fcontent;
}


inline bool IsNumber(char a) { return a <= '9' && a >= '0'; };
inline bool IsWhitespace(char c) { return (c == ' ' || c == '\t' || c == '\r'); }

int Day()
{
    char* text = Helper_ReadAllText("Assets/AOC1.txt");

    char* curr = text;
    int maxElf = 0;
    while (*curr != -1 && *curr)
    {
        int value = 0, total = 0;
        // parse, and accumulate elf values
        while (IsNumber(*curr))
        {
            while (IsNumber(*curr))
                value = value * 10 + (*curr++ - '0');
            total += value;
            value = 0;
            while (IsWhitespace(*curr)) curr++;
            curr += *curr == '\n';
        }

        while (IsWhitespace(*curr)) curr++; // skip white space and new line
        curr += *curr == '\n';
        maxElf = Max(maxElf, total); // find maximum elf
    }
    printf("max elf: %d", maxElf);
    free(text);
    return maxElf;
}

int Day2()
{
    char* text = Helper_ReadAllText("Assets/AOC2.txt");
    char* curr = text;

    int ourScore = 0;
    // parse each line
    while (*curr)
    {
        // char to rock paper scisor conversation, 'A'-'C' to 0, 2
        int opponent = *curr++ - 'A'; curr++;
        int ourself = *curr++ - 'X'; curr++;

        enum { Rock, Paper, Scisor };
        int weWin = ourself == Rock && opponent == Scisor;
        weWin |= (ourself == Paper && opponent == Rock);
        weWin |= (ourself == Scisor && opponent == Paper);

        ourScore += (opponent == ourself) * 3; // +3 if same 
        ourScore += weWin * 6 + ourself + 1; // +6 for win, +ourHand, +1 for 0 indexed
        // skip whitespace
        while (IsWhitespace(*curr) || *curr == '\n') curr++;
    }

    printf("our score: %d", ourScore);
    free(text);
    return ourScore;
}

int Day3()
{
    FILE* file = fopen("Assets/AOC3.txt", "r");
    char line[128] = { 0 };
    int prioritySum = 0;

    while (fgets(line, sizeof(line), file))
    {
        uint64 existanceMap = 0ull; // we will use this as unordered_set<char>, or bitset
        size_t i = 0, n = strlen(line);

        // parse first half of text. existanceMap.insert(line[i]);
        for (; i < (n / 2); ++i) existanceMap |= 1ull << (line[i] - 'A');
        // find same character index in second part,  if (existanceMap.find(line[i]))
        while (i < n)
            if (existanceMap & (1ull << (line[i++] - 'A'))) break;

        prioritySum += line[i] <= 'Z' ? line[i] - 'A' + 27 : line[i] - 'a' + 1; // convert character to point ve earn
    }
    fclose(file);
    printf("prioritySum: %d", prioritySum);
    return prioritySum;
}

int Day3Part2()
{
    char const* const fileName = "Assets/AOC3.txt";
    FILE* file = fopen(fileName, "r");
    char line[128] = { 0 };
    int prioritySum = 0;
    // parse 3 line until end of file
    // recommended ascii table for better understanding.
    while (fgets(line, sizeof(line), file)) // read first line
    {
        // 64bit mask's for upper-lower case characters, same as unordered_set<char>
        uint64 amask = 0u, bmask = 0u, cmask = 0u;

        const char* curr = line;
        // create first line's mask
        // -'A' because we will map characters into 64bit.
        // imagine something like this: 101010111111100000001010101111000010101111
        //              upper case start^       lower case start^
        while (*curr != '\n') amask |= 1ull << (*curr++ - 'A');

        fgets(line, sizeof(line), file); curr = line; // read second line
        while (*curr != '\n') bmask |= 1ull << (*curr++ - 'A');

        fgets(line, sizeof(line), file); curr = line; // read third line
        while (*curr != '\n') cmask |= 1ull << (*curr++ - 'A');

        uint64 intersection = amask & bmask & cmask; // set intersection

        // detect intersection index, if upper case range will be: 0 <= x <= 26. if lower case int('a' - 'A') <= x < 64
        uint64 tzcnt = _tzcnt_u64(intersection); // __builtin_ctz, returns intersection bit index, only one index is common in this 3 line
        /* if upper case*/
        if (tzcnt < 30) prioritySum += tzcnt + 27;
        else prioritySum += tzcnt - ('a' - 'A') + 1; // convert 'a'-'z' to 0-26
    }

    fclose(file);

    printf("our score: %d", prioritySum);
    // delete[] text;
    return prioritySum;
}
struct Range {
    int begin = 0, end = 0;
    Range() { }
    Range(int b, int e) : begin(b), end(e) { }
};

int Day4()
{
    int numFullContain = 0;
    char line[64];
    FILE* file = fopen("Assets/AOC4.txt", "r");

    while (fgets(line, sizeof(line), file))
    {
        Range first, second;

        fscanf(file, "%i-%i,%i-%i", &first.begin, &first.end, &second.begin, &second.end);

        if ((first.begin <= second.begin && first.end >= second.end)
            || (second.begin <= first.begin && second.end >= first.end))
            numFullContain++;
    }

    fclose(file);
    printf("num full contain: %d", numFullContain);
    return numFullContain;
}

int Day5()
{
    FILE* file = fopen("Assets/AOC5.txt", "r");

    char line[64];
    char stacks[10][64] = { 0 }; // if input has more than 10 group, we can easily increase number of stacks here
    char numPacks[10] = { 0 };

    char numStacks = 0;
    // parse each group of box column's
    while (fgets(line, sizeof(line), file))
    {
        if (line[1] == '1') break;
        char len = (char)strlen(line), stackIndex = 0;

        for (char i = 1; i < len; i += 4, stackIndex++)
        {
            if (line[i] <= 'Z' && line[i] >= 'A') // is character and not whitespace?
                stacks[stackIndex][numPacks[stackIndex]++] = line[i];
        }
        numStacks = Max(stackIndex, numStacks);
    }
    // reverse each stack, because we parsed reversly
    for (char i = 0; i < numStacks; ++i)
        for (char j = 0, k = numPacks[i] - 1; j < k; j++, --k)
            Swap(stacks[i][j], stacks[i][k]);

    fgets(line, sizeof(line), file); // parse empty line that seperates stacks and commands

    while (fgets(line, sizeof(line), file)) // read each line
    {
        char amount = 0, from = 0, to = 0;
        //sscanf(line, "move %i from %i to %i", &amount, &from, &to);
        const char* curr = line + 5;
        // atoi. parse line, amount and to.
        while (*curr != ' ') amount = amount * 10 + (*curr++ - '0'); curr += 6; // +6 for seeking begining of the from index: strlen(' from ')
        while (*curr != ' ') from = from * 10 + (*curr++ - '0'); curr += 4; // +4 for seeking begining of the to index: strlen(' to ')
        while (*curr && *curr != '\n') to = to * 10 + (*curr++ - '0');

        from--, to--; // decrese because 1 indexed
        // move from to to by amount
#ifdef PART1
        for (int i = 0; i < amount; ++i) {
            stacks[to][numPacks[to]++] = stacks[from][--numPacks[from]];
        }
#else
        char end = numPacks[to] + amount - 1;
        numPacks[to] += amount;
        while (amount--) {
            stacks[to][end--] = stacks[from][--numPacks[from]];
        }
#endif
    }

    // collect top blocks, I will use line as result text
    for (char i = 0; i < numStacks; ++i) {
        line[i] = stacks[i][numPacks[i] - 1];
    }
    line[numStacks] = 0;
    fclose(file);
    printf("top stacks: %s", line);
    return 0;
}

int Day6()
{
    char* text = Helper_ReadAllText("Assets/AOC6");
    const char* curr = text;

    unsigned numSignals = 0u, numUnique = 0u;
    char charMap[28] = { 0 }; // unordered_set<char> 
    // ring buffer that keeps track of currently visited characters 
    char lastFour[8] = { 27, 27, 27, 27, 27, 27, 27, 27 };
    //                   ^curFour         ^lastidx
    char curFour = 4, lastIdx = 0;

    // sliding window aproach
    while (*curr && numUnique != 4u)
    {
        // remove last
        char lastChar = lastFour[lastIdx++ & 7]; // 'i & 7 = i % 8' find's last visited character = visited[curr - 4]
        if (charMap[lastChar] > 1) charMap[lastChar]--; // decrease amount if we have more character's inside
        else if (charMap[lastChar] == 1) numUnique--, charMap[lastChar]--; // if is this last exist character in the map, we reduce num of unique

        // read new character
        char newChar = *curr++ - 'a';
        if (charMap[newChar] == 0) numUnique++; // increase if this is unique 
        lastFour[curFour++ & 7] = newChar; // %8
        charMap[newChar]++;
        numSignals++;
    }
    free(text);
    printf("processed signals: %d \n", numSignals);
    return 0;
}

constexpr uint MaxSubFolders = 12;
constexpr int MaxFolders = 256;

struct Folder
{
    uint hash; // name hash
    uint size;
    uint numFolders;
    ushort subFolders[MaxSubFolders]; // index array for indexing array location in folders array
};

int Day7()
{
    Folder folders[MaxFolders];
    short numFolders = 1u;

    FILE* file = fopen("Assets/AOC7.txt", "r");

    char line[32];
    fgets(line, sizeof(line), file); // read first line: $ cd /
    folders[0].size = 0;
    folders[0].numFolders = 0;

    short parentPaths[40] = { 0 };
    short parentIndex = 0;
    short currentFolderIdx = 0;

    while (fgets(line, sizeof(line), file))
    {
    parse_command:
        if (line[2] == 'c') // cd command
        {
            if (line[5] == '.') { // cd..
                currentFolderIdx = parentPaths[--parentIndex]; // set current path to parent path and decrease number of parent paths
                continue;
            }
            uint folderHash = PathToHash(line + 5);
            Folder& currentFolder = folders[currentFolderIdx];
            // find path that specified with input, in subfolders
            for (uint i = 0u; i < currentFolder.numFolders; ++i)
            {
                Folder& subFolder = folders[currentFolder.subFolders[i]];
                if (subFolder.hash == folderHash)
                {
                    parentPaths[parentIndex++] = currentFolderIdx;
                    parentPaths[parentIndex] = currentFolderIdx = currentFolder.subFolders[i];
                    break;
                }
            }
            continue;
        }
        // else if (line[2] == 'l') // ls command
        while (fgets(line, sizeof(line), file))
        {
            if (line[0] == 'd') // new dir
            {
                // create new folder
                uint newFolderIdx = numFolders++;
                folders[newFolderIdx].size = 0;
                folders[newFolderIdx].numFolders = 0;
                folders[newFolderIdx].hash = PathToHash(line + 4);

                Folder& currentFolder = folders[currentFolderIdx];
                currentFolder.subFolders[currentFolder.numFolders++] = newFolderIdx; // add new folder to folders array
            }
            else if (IsNumber(line[0])) // new file
            {
                const char* curr = line;
                uint fileSize = 0u;
                // string to int, atoi
                while (*curr != ' ') fileSize = fileSize * 10u + (*curr++ - '0');

                // increase size of all parent folders
                short currParent = parentIndex;
                while (currParent >= 0) {
                    folders[parentPaths[currParent--]].size += fileSize;
                }
            }
            else if (line[0] == '$') goto parse_command;
        }
    }

    uint result = 0u;
#ifndef PART1 
    for (short i = 0; i < numFolders; ++i)
    {
        if (folders[i].size < 100'000) result += folders[i].size;
    }
#else
    uint minRemovedSize = 0, minFile = LONG_MAX;
    uint needSpace = 30000000 - (70000000 - folders[0].size);

    for (short i = 0; i < numFolders; ++i)
    {
        if (folders[i].size >= needSpace && folders[i].size < minFile)
        {
            minFile = folders[i].size;
        }
    }
    result = minFile;
#endif
    printf("result: %d ", result);
    return result;
}

int Day8()
{
    char* text = ReadAllFile("Assets/AOC8.txt");
    bool visited[100][100] = { 0 };

    // look from left to right
    for (char y = 0; y < 99; ++y)
        for (char x = 0, maxSoFar = 0; x < 100; ++x) {
            char current = text[y * 100 + x];
            if (current > maxSoFar) maxSoFar = current, visited[y][x] = true; else break;
        }
    // look from right to left
    for (char y = 98; y >= 0; --y)
        for (char x = 99, maxSoFar = 0; x >= 0; --x) {
            char current = text[y * 100 + x];
            if (current > maxSoFar) maxSoFar = current, visited[y][x] = true; else break;
        }
    // look from top to bottom
    for (char x = 0; x < 100; ++x)
        for (char y = 0, maxSoFar = 0; y < 99; ++y) {
            char current = text[y * 100 + x];
            if (current > maxSoFar) maxSoFar = current, visited[y][x] = true; else break;
        }
    // look from bottom to top
    for (char x = 99; x >= 0; --x)
        for (char y = 98, maxSoFar = 0; y >= 0; --y) {
            char current = text[y * 100 + x];
            if (current > maxSoFar) maxSoFar = current, visited[y][x] = true; else break;
        }

    int numVisible = 0;
    for (char x = 0; x < 100; ++x) for (char y = 0; y < 99; ++y)
        numVisible += visited[x][y];

    printf("num visible: %d", numVisible);
    free(text);
    return 0;
}
#include <chrono> 
int Day10()
{
    const char* text = ReadAllFile("Assets/AOC10.txt");
    const char* curr = text;

    char screen[6][40];
    for (int i = 0; i < 6; ++i) FillN(screen[i], '.', 40);
    
    int x = 1;
    short cycle = 1, targetCycle = 20; // target cycle will be 20 and we will add it 40 for the pattern: 20-60-100-140-180-120
    int resultSum = 0;

    const auto processTargetReached = [&]() {
        int row = Min(5,(cycle - 1) / 40);
        int col = (cycle - 1) % 40;
        bool on = x >= col - 1 && x <= col + 1;
        screen[row][col] = on ? '#' : '.';

        system("@cls||clear"); // clear console

        for (int i = 0; i < 6; ++i) { // print screen display
            for (int j = 0; j < 40; ++j)
                printf("%c", screen[i][j]);
            printf("\n");
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(4ms);
        
        if (cycle == targetCycle) {
            resultSum += targetCycle * x;
            targetCycle += 40;
        }
    };
    
    while (*curr && cycle <= 220)
    {
        cycle++;
        bool isAddxInstruction = *curr == 'a';
        curr += 5; // +5 because if addx: go to number index. for noop: jumping from beginning of the instruction to end of line

        if (isAddxInstruction)
        {
            processTargetReached();
            cycle++;
            x += ParseInt(curr);
        } // else noop instruction

        processTargetReached();
        while (IsWhitespace(*curr) || *curr == '\n') curr++;
    }
    printf("result: %d", resultSum);
    return 0;
}
