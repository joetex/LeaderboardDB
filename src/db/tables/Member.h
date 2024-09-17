
#pragma once
#include <string>
#include <any>
#include <vector>
//#include "db/datastructures/ska_flat_hash_map.h"

#include "db/datastructures/json.h"
using json = nlohmann::json;
using namespace std;

enum class ColumnType {
	INT8,
	UINT8,
	INT16,
	UINT16,
	LONG,
	ULONG,
	FLOAT,
	DOUBLE,
	VARCHAR,
	BOOLEAN,
	TIMESTAMP,
};

struct MemberColumnDef {
	ColumnType type;
	string name;
	unsigned int maxByteLength;
};

//typedef ska::flat_hash_map<unsigned long, MemberColumnDef> MemberColumns;

class MemberTable {
public:
	MemberTable(string name) :tableName(name) {

	}
	string tableName;
	std::vector<MemberColumnDef> columns;
	std::vector<json> rows;
};



struct MemberStat {
	unsigned long memberId;
	unsigned long statId;
};