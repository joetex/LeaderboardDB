#pragma once

#include <string>
#include <variant>
#include <optional>
#include <chrono>

using namespace std;

enum class StatType {
	None = 0,
	ScoreInt,
	ScoreUInt,
	ScoreLong,
	ScoreULong,
	ScoreFloat,
	ScoreDouble,
	Time,
	Average,
	StringCount
};

typedef variant<int, unsigned int, long, unsigned long, float, double> StatValue;

struct StatColumnDef {
	unsigned long memberId;
	StatValue statValue;
	optional<string> statString;
	chrono::milliseconds insertTime;
};

typedef vector<StatColumnDef> StatColumns;

struct StatTable {
	unsigned long statId;
	string statName;
	string statAbbr;
	StatType statType;
	StatColumns columns;
};
