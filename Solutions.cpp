
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
    while (*curr != -1 && *curr != 0)
    {
        int value = 0, total = 0;

        while (IsNumber(*curr))
        {
            while (IsNumber(*curr))
                value = value * 10 + (*curr++ - '0');
            total += value;
            value = 0;
            while (IsWhitespace(*curr)) curr++;
            curr += *curr == '\n';
        }

        while (IsWhitespace(*curr)) curr++;
        curr += *curr == '\n';
        maxElf = Max(maxElf, total);
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

    while (*curr)
    {
        int opponent = *curr++ - 'A'; curr++;
        int ourself = *curr++ - 'X'; curr++;

        enum { Rock, Paper, Scisor };
        int weWin = ourself == Rock && opponent == Scisor;
        weWin |= (ourself == Paper && opponent == Rock);
        weWin |= (ourself == Scisor && opponent == Paper);

        ourScore += (opponent == ourself) * 3;
        ourScore += weWin * 6 + ourself + 1;

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
        uint64 existanceMap = 0ull;
        size_t i = 0, n = strlen(line);

        for (; i < (n / 2); ++i) existanceMap |= 1ull << (line[i] - 'A');
        for (; i < n; ++i)   if (existanceMap & (1ull << (line[i] - 'A'))) break;

        prioritySum += line[i] <= 'Z' ? line[i] - 'A' + 27 : line[i] - 'a' + 1;
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

    while (fgets(line, sizeof(line), file))
    {
        uint64 amask = 0u, bmask = 0u, cmask = 0u;

        const char* curr = line;
        while (*curr != '\n') amask |= 1ull << (*curr++ - 'A');

        fgets(line, sizeof(line), file); curr = line;
        while (*curr != '\n') bmask |= 1ull << (*curr++ - 'A');

        fgets(line, sizeof(line), file); curr = line;
        while (*curr != '\n') cmask |= 1ull << (*curr++ - 'A');

        uint64 intersection = amask & bmask & cmask;

        uint64 tzcnt = _tzcnt_u64(intersection); // __builtin_ctz 

        if (tzcnt < 30) prioritySum += tzcnt + 27;
        else            prioritySum += tzcnt - ('a' - 'A') + 1;
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
    char stacks[10][64] = { 0 }; // we can easily increase number of stacks here
    char numPacks[10] = { 0 };

    char numStacks = 0;

    while (fgets(line, sizeof(line), file))
    {
        if (line[1] == '1') break;
        char len = (char)strlen(line), stackIndex = 0;

        for (char i = 1; i < len; i += 4, stackIndex++)
        {
            if (line[i] <= 'Z' && line[i] >= 'A')
                stacks[stackIndex][numPacks[stackIndex]++] = line[i];
        }
        numStacks = Max(stackIndex, numStacks);
    }
    // reverse stacks
    for (char i = 0; i < numStacks; ++i)
        for (char j = 0, k = numPacks[i] - 1; j < k; j++, --k)
            Swap(stacks[i][j], stacks[i][k]);

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file))
    {
        char amount = 0, from = 0, to = 0;
        //sscanf(line, "move %i from %i to %i", &amount, &from, &to);
        const char* curr = line + 5;
        while (*curr != ' ') amount = amount * 10 + (*curr++ - '0'); curr += 6;

        while (*curr != ' ') from = from * 10 + (*curr++ - '0'); curr += 4;

        while (*curr && *curr != '\n') to = to * 10 + (*curr++ - '0');

        from--, to--; // decrese because 1 indexed
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

    // collect top blocks
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
    char charMap[28] = { 0 };
    char lastFour[8] = { 27, 27, 27, 27, 27, 27, 27, 27 };
    char curFour = 4, lastIdx = 0;

    // sliding window aproach
    while (*curr && numUnique != 4u)
    {
        // remove last
        char lastChar = lastFour[lastIdx++ & 7];
        if (charMap[lastChar] > 1) charMap[lastChar]--;
        else if (charMap[lastChar] == 1) numUnique--, charMap[lastChar]--;

        // read new character
        char newChar = *curr++ - 'a';
        if (charMap[newChar] == 0) numUnique++;
        lastFour[curFour++ & 7] = newChar;
        charMap[newChar]++;
        numSignals++;
    }
    free(text);
    printf("processed signals: %d \n", numSignals);
    return 0;
}

constexpr uint MaxSubFolders = 12;
constexpr int MaxFolders = 400;

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

    char line[64];
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
            if (line[5] == '.') // cd..
            {
                currentFolderIdx = parentPaths[--parentIndex];
            }
            else
            {
                uint folderHash = PathToHash(line + 5);
                Folder& currentFolder = folders[currentFolderIdx];

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
            }
        }
        else if (line[2] == 'l') // ls command
        {
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

                    while (*curr != ' ') fileSize = fileSize * 10u + (*curr++ - '0');

                    // increase size of all parent folders
                    short currParent = parentIndex;

                    while (currParent >= 0)
                    {
                        folders[parentPaths[currParent--]].size += fileSize;
                    }
                }
                else if (line[0] == '$') goto parse_command;
            }
        }
    }

    // accumulate for result
    uint result = 0ul;
    for (short i = 0; i < numFolders; ++i)
    {
        if (folders[i].size < 100'000) result += folders[i].size;
    }
    fclose(file);
    printf("result: %d", result);
    return result;
}
