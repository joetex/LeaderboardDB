#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>
#include <map>

#include "db/FileDiskManager.h"


typedef std::basic_ofstream<unsigned char, std::char_traits<unsigned char> > uofstream;

/**
 * Write memory to Disk
 * Manages writing Members, Stats, and Leaderboards to disk
 */
class Pager {

	FileDiskManager *fdm = new FileDiskManager();

	enum class FileType {
		Table = 1,
		BTreeType =2 
	};

	struct FileHeader {
		FileType type;
		std::string filename;
		std::string filepath;
	};

	struct Table {
		std::string name;
	};

	struct DBFile {
		std::string name;
		std::string filename;
		std::string filepath;
		std::map < std::string, Table> tables;
	};

	std::map<std::string, DBFile> dbs;

	std::map<std::string, FileHeader> files;

public:
	Pager() {

	}

	std::filesystem::path cwd() { return std::filesystem::current_path(); }
	wchar_t sep() { return std::filesystem::path::preferred_separator; }


	bool createDB(std::string name) {
		try {
			std::filesystem::create_directory(cwd() / "data");
			int PAGE_SIZE = (int)fdm->getPageSize();
			unsigned char* pageBytes = new unsigned char[PAGE_SIZE];
			std::fill(pageBytes, pageBytes + PAGE_SIZE, 0);

			DBFile dbfile;
			dbfile.name = name;
			dbfile.filename = name + ".db";

			std::filesystem::path filepath = cwd() / "data" / dbfile.filename;// std::format("{}{}data{}{}{}", cwd(), sep(), sep(), name, ".sseq");
			dbfile.filepath = filepath.string();

			fdm->createFile(filepath, pageBytes, PAGE_SIZE);

			dbs.insert({ name, dbfile });
			return true;
		}
		catch (std::exception e) {
			std::cout << e.what() << endl;
			return false;
		}
	}

	bool addTableToDB(std::string db, Table table) {
		try {

			DBFile dbfile = dbs[db];
			dbfile.tables.insert({ db, table });


			//int fd = open(dbfile.filepath.c_str(), 0_RDWR, 0777);

		}
		catch (std::exception e) {
			std::cout << e.what() << endl;
			return false;
		}
		return true;
	}

	/**
	First page has metadata
	// Byte		0		= type of file (1=sequential, 2=btree)
	// Bytes	1-4		= Page count of data
	*/
	bool createTable(std::string db, std::string name) {

		try {
			int PAGE_SIZE = (int)fdm->getPageSize();
			unsigned char* pageBytes = new unsigned char[PAGE_SIZE];
			std::fill(pageBytes, pageBytes + PAGE_SIZE, 0);

			//std::array<unsigned char, (size_t)fdm->getPageSize()> pageBytes = {};
			pageBytes[0] = 1;

			std::filesystem::create_directory(cwd() / "data");

			FileHeader header;
			header.filename = name + ".sseq";
			header.type = FileType::Table;
			std::filesystem::path filepath = cwd() / "data" / header.filename;// std::format("{}{}data{}{}{}", cwd(), sep(), sep(), name, ".sseq");
			header.filepath = filepath.string();

			fdm->createFile(filepath, pageBytes, PAGE_SIZE);

			Table table;
			table.name = name;

			addTableToDB(db, table);
		}
		catch (std::exception e) {
			std::cout << e.what() << endl;
			return false;
		}


		return true;
	}

	template <typename T>
	bool insertRow(std::string name, unsigned int start, unsigned int end, T** rows) {
		return false;
	}

	bool createIndex(std::string name) {
		int PAGE_SIZE = (int)fdm->getPageSize();
		unsigned char* pageBytes = new unsigned char[PAGE_SIZE];
		std::fill(pageBytes, pageBytes + PAGE_SIZE, 0);
		
		pageBytes[0] = 1;

		//std::string filepath = std::format("{}{}data{}{}", cwd(), sep(), sep(), name, ".sidx");
		std::filesystem::path filepath = cwd() / "data" / name / ".sidx";// std::format("{}{}data{}{}{}", cwd(), sep(), sep(), name, ".sseq");


		std::ofstream myfile;
		myfile.open(filepath, std::ios_base::binary);
		myfile << 1;
		myfile << 0;
		myfile.close();

		FileHeader header;
		header.filename = name;
		header.filepath = filepath.string();
		header.type = FileType::Table;

		return true;
	}

	template <typename T>
	bool updateIndex(std::string name, T* root) {
		return false;
	}

	/**
	 Write dirty nodes to disk.  Each node should be equal or less than page size
	*/
	template <typename T>
	bool WriteBTree(T* root) {
		return false;
	}

};