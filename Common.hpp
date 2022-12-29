#include <stdio.h>

typedef unsigned long long ulong;
typedef unsigned int uint;
typedef unsigned long long uint64;
typedef unsigned short ushort;

inline bool IsNumber(char a) { return a <= '9' && a >= '0'; };
inline bool IsWhitespace(char c) { return (c == ' ' || c == '\t' || c == '\r'); }
template<typename T> inline void Swap(T& a, T& b) { T t = a; a = b, b = t;  }

constexpr inline uint WangHash(uint s) {
	s = (s ^ 61u) ^ (s >> 16u);
	s *= 9, s = s ^ (s >> 4u);
	s *= 0x27d4eb2du;
	s = s ^ (s >> 15u);
	return s;
}

constexpr inline uint StringToHash(const char* str, uint hash = 0)
{
	while (*str)
		hash = *str++ + (hash << 6u) + (hash << 16u) - hash;
	return hash;
}

constexpr inline uint PathToHash(const char* str)
{
	uint hash = 0u, idx = 0u, shift = 0u;
	while (str[idx] && idx < 4u)
		hash |= uint(str[idx]) << shift, shift += 8u, idx++;
	return StringToHash(str + idx, WangHash(hash));
}

template<typename T>
inline T ParseNumber(const char*& curr)
{
	while (IsWhitespace(*curr)) curr++; // skip whitespace
	T val = 0;
    bool negative = false;
    if (*curr == '-') curr++, negative = true;
	while (IsNumber(*curr))
		val = val * 10 + (*curr++ - '0');
    if (negative) val = -val;
    return val;
}

template<typename T>
inline void ParseNumberRef(const char*& curr, T& val)
{
	while (IsWhitespace(*curr)) curr++; // skip whitespace
	while (IsNumber(*curr))
		val = val * 10 + (*curr++ - '0');
}

[[deprecated("use ParseNumber<T> instead.")]]
inline int ParseInt(const char*& curr) // obsolute use ParseNumber instead
{
	while (IsWhitespace(*curr)) curr++; // skip whitespace
	int instructionVal = 0;
    bool negative = false;
    if (*curr == '-') curr++, negative = true;
	while (IsNumber(*curr))
        instructionVal = instructionVal * 10 + (*curr++ - '0');
    if (negative) instructionVal = -instructionVal;
    return instructionVal;
}

template<typename T>
inline bool TryParse(T& val, const char*& curr)
{   // additional checks
	if (*curr == 0 || *curr == '\n') return false;
	while (IsWhitespace(*curr)) curr++; 
	if (!IsNumber(*curr) && *curr != '-') return false;
	val = ParseNumber<T>(curr);
	return true;
}

inline bool StartsWith(const char*& curr, const char* str)
{
	const char* currStart = curr;
	while (IsWhitespace(*curr)) curr++;
	if (*curr != *str) return false;
	while (*str && *curr++ == *str++);
	bool isEqual = *str == 0;
	if (!isEqual) curr = currStart;
	return isEqual;
}

template<typename T>
inline void BubleSort(T* a, int len)
{
	for (int i = 0; i < len; ++i)
		for (int j = i+1; j < len; ++j)
			if (a[j] < a[i]) Swap(a[i], a[j]);
}

template<typename T> inline void FillN(T* arr, T val, int n)
{
    for (int i = 0; i < n; ++i) arr[i] = val;
}

inline char* ReadAllFile(const char* fileName, int* numCharacters = 0)
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

    char* fcontent = (char*)calloc(fsize+1, 1);
    fread(fcontent, 1, fsize, fp);
    fclose(fp);
   
    if (numCharacters) *numCharacters = fsize;
    return fcontent;
}
