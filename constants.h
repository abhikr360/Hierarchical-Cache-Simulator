#ifndef constants_included
#define constants_included

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <algorithm>
#include <climits>
#include <unordered_map>


#include <cassert>
#include <cstdio>

using namespace std;

#ifndef INVALID
#define INVALID -1
#endif

#ifndef pb
#define pb push_back
#endif

#ifndef mp
#define mp make_pair
#endif


#ifndef LOAD
#define LOAD 0
#endif

#ifndef STORE
#define STORE 1
#endif

#ifndef INSTRUCTION
#define INSTRUCTION 2
#endif



typedef unsigned long long ull;
typedef long long ll;

enum replacement_policy
{
	BELADY,
	LRU
};
#endif