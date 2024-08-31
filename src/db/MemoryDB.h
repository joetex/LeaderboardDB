#pragma once
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <any>
#include <memory>
#include <iostream>

#include "tables/Member.h"
#include "tables/Stat.h"
#include "tables/Leaderboard.h"
#include "datastructures/ska_flat_hash_map.h"

#include "db/Pager.h"

using namespace std;

class MemoryDB {
	ska::flat_hash_map<std::string, MemberTable*> memberTables;
	ska::flat_hash_map<std::string, StatTable> statTables;

	Pager* pager = new Pager();

public:
	MemoryDB() {
		LoadMembers();

		pager->createDB("mydb");

		pager->createTable("mydb", "test");
	}

	void LoadMembers() {
		MemberTable *table = new MemberTable("testTable");
		memberTables.emplace(table->tableName, table);
		/*MemberColumns cols;
		CreateMember(1, cols);
		CreateMember(2, cols);
		CreateMember(3, cols);*/
		
	}

	/**
	
	Display All Tables
	\param[int]	a test
	\param[std::string] another
	\returns this is result
	*/
	void Display() {
		//map<string, MemberColumns>::iterator it;
		/*for (it = memberTables.begin(); it != memberTables.end(); it++)
		{
			cout <<"Member: " << it->first << endl;
		}*/
	}
	// Member Metadata
	// {tableName} std::string - name to use when referencing this member table
	// {MemberColumns} vector<MemberColumns>
	/**
	Create a Member Table
	@param [string] tableName
	@param[MemberColumns] columns
	@note test 1234
	@return onetwothreee
	*/
	void CreateMember(std::string tableName, json row) {
		MemberTable *table = memberTables[tableName];
		table->rows.push_back(row);
	}

	// Statistic Definition
	// {statName} std::string - name to use when referencing this statistic table
	// {statType} enum StatType - the type of statistic (i.e. ScoreInt, ScoreFloat, Time, Average, StringCount, etc.) 
	// Table columns: 
	// long statId, long memberId, long statTime, <int|uint|long|ulong|float|double> statValue, [string statString]
	void CreateStatistic(std::string statName, string statAbbr, StatType statType) {
		StatTable table;
		table.statName = statName;
		table.statAbbr = statAbbr;
		table.statType = statType;
		statTables[statName] = table;
	}

	// Leaderboard Definition
	// {leaderboardName} string - name of the leaderboard table
	// {memberTable} std::string - name of the member table to target
	// {statTable} std::string - name of the stat table to target
	// {startTime} long - start timestamp against stat table
	// {endTime} long - end timestamp against stat table
	// {rules} LeaderboardRules - object defining the rules of this leaderboard (i.e. Start Time, End Time)
	// Table columns:
	// int leaderboardId, long memberId, long statIdBest, long statIdLast
	void CreateLeaderboard() {

	}


};