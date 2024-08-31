#pragma once

#include <fstream>
#include <filesystem>

//#include <boost/filesystem.hpp>
//#include <boost/spirit/home/x3.hpp>
#define WIN32_LEAN_AND_MEAN

#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#if WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#endif


class FileDiskManager {

	long PAGE_SIZE = 4096;
public:
	FileDiskManager() {
		init();
	}

	void init() {
		updatePageSize();
	}

	long constexpr getPageSize() {
		return PAGE_SIZE;
	}
	long updatePageSize() {
#if __linux__
		long sz = sysconf(_SC_PAGESIZE);
		printf("%s %ld\n\n", "Linux PageSize[Bytes] :", sz);
		PAGE_SIZE = sz;
#elif WIN32
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		printf("%s %d\n\n", "Windows PageSize[Bytes] :", sysInfo.dwPageSize);
		PAGE_SIZE = sysInfo.dwPageSize;
#endif
		return PAGE_SIZE;
	}

	bool createFile(std::filesystem::path filepath, unsigned char*data, std::streamsize size) {
		std::ofstream myfile;
		
		myfile.open(filepath, std::ios::binary | std::ios::in | std::ios::out);
		if (size > 0) {
			myfile.write(reinterpret_cast<char*>(data), size);
		}
		myfile.close();
		return true;
	}

	bool writeFile(std::filesystem::path filepath, unsigned int startPos, unsigned char* data, std::streamsize size) {
		//std::ofstream myfile;
		//boost::iostreams::basic_mapped_file_params<std::filesystem::path> param(filepath); // template param

		//boost::filesystem::path p(filepath);

		//param.path = filepath;
		//param.new_file_size = PAGE_SIZE;
		//param.flags = boost::iostreams::mapped_file::mapmode::readwrite;
		boost::iostreams::mapped_file_source file;// (boost::iostreams::detail::path(filepath.string()));

		file.open<boost::iostreams::detail::path>(boost::iostreams::detail::path(filepath.string()), size, (long long)startPos);

		if (file.is_open()) {

		}
		/*myfile.open(filepath, std::ios::binary | std::ios::app | std::ios::out);
		if (size > 0) {
			myfile.seekp(startPos, ios::beg);
			myfile.write(reinterpret_cast<char*>(data), size);
		}
		myfile.close();*/
		return true;
	}

	bool readFilePage(std::filesystem::path filepath, unsigned int pageNumber, unsigned char* data) {
		std::ifstream myfile;

		myfile.open(filepath, std::ios::binary | std::ios::in);
	
			myfile.seekg(pageNumber * PAGE_SIZE, ios::beg);
			//myfile.read(reinterpret_cast<char*>(data), size);
		
		myfile.close();
		return true;
	}

};