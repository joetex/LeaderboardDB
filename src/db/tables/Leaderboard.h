#pragma once

#include <string>
#include <chrono>
#include "db/datastructures/ska_flat_hash_map.h"

using namespace std;

struct LeaderboardColumnDef {
	unsigned long memberId;
	unsigned long statId;
};

typedef ska::flat_hash_map<unsigned int, LeaderboardColumnDef> LeaderboardColumns;

struct LeaderboardTable {
	string leaderboardName;
	string memberName;
	string statName;
	chrono::milliseconds startTime;
	chrono::milliseconds endTime;
	LeaderboardColumns columns;
};

