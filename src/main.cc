// MovStat.cpp : Defines the entry point for the application.
//


#include <cmdline/CommandLoop.h>
#include "db/Engine.h"
#include <string>
#include <memory>
#include <format>
#include <algorithm>
#include <random>
#include <ranges>

//#include "db/datastructures/HashTree.h"
#include "db/datastructures/Base62.h"
//#include "db/datastructures/BTree.h"

//#include "db/datastructures/MinMaxSort.h"
#include "db/datastructures/MinMaxTree.h"

using namespace std;
//#include "fc/btree.h"

struct BTData {
	int key;
	std::string name;
};

uint32_t LilEntropy() {
	//Gather many potential forms of entropy and XOR them
	const  uint32_t my_seed = 1273498732; //Change during distribution
	static uint32_t i = 0;
	static std::random_device rd;
	const auto hrclock = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	const auto sclock = std::chrono::system_clock::now().time_since_epoch().count();
	auto* heap = malloc(1);
	const auto mash = my_seed + rd() + hrclock + sclock + (i++) +
		reinterpret_cast<intptr_t>(heap) + reinterpret_cast<intptr_t>(&hrclock) +
		reinterpret_cast<intptr_t>(&i) + reinterpret_cast<intptr_t>(&malloc) +
		reinterpret_cast<intptr_t>(&LilEntropy);
	free(heap);
	return mash;
}

void SeedGenerator(std::mt19937& mt) {
	std::uint_least32_t seed_data[std::mt19937::state_size];
	std::generate_n(seed_data, std::mt19937::state_size, std::ref(LilEntropy));
	std::seed_seq q(std::begin(seed_data), std::end(seed_data));
	mt.seed(q);
}

int randint(int min, int max) {
	return rand() % ((max - min) + 1) + min;
}

unsigned int totalTreeNodes = 0;
int main()
{
	cout << "=== LeaderboardDB v1.0 === " << endl;

	srand(10);

	std::mt19937 mt;
	SeedGenerator(mt);

	std::uint_least32_t seed_data[std::mt19937::state_size];
	std::generate_n(seed_data, std::mt19937::state_size, std::ref(LilEntropy));
	std::seed_seq q(std::begin(seed_data), std::end(seed_data));

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 1000000);

	std::vector<int> inserted;
	//BTree<BTData> *bt = new BTree<BTData>(256);
	//frozenca::BTreeSet<std::int64_t, 64> bt;// = new frozenca::BTreeMap<int, BTData, 128>();
	{

		typedef MinMaxTree<128, true> MMSort;

		MMSort minmax;

			unsigned int insertTotal = 1000000;

			//for (int i = 0; i <= insertTotal; i++) {
				for (int i = insertTotal; i >= 0; i--) {
				inserted.push_back(i);// dist6(mt));
				//inserted.push_back( dist6(mt));
			}
				insertTotal = inserted.size();
		auto start = chrono::high_resolution_clock::now();
		{
			int j = 0;
			for (int i = 0; i < insertTotal; i++) {
			//for (int i = insertTotal; i > 0; i--) {
			 //minmax.insert(i, i);
				
				minmax.insert(inserted[i], i);
			}
			auto finish = chrono::high_resolution_clock::now();
			cout << "Insert " << insertTotal << " : " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";

		}

		//minmax.display();

		start = chrono::high_resolution_clock::now();
		{
			int findValue = 0;
			int findCount = 10;
			std::ostringstream os;

			cout << "Show top ranks, lower is better \n";
			//for (int j=0; j < 100000; j++) {
			std::vector<std::tuple<unsigned int, unsigned long long, unsigned long long>> ranks = minmax.range(findValue, findCount);
				for (std::tuple<unsigned int, unsigned long long, unsigned long long> rank : ranks) {
					
					os << "Rank [" 
						<< std::get<0>(rank) 
						<< "]: score="
						<< std::get<1>(rank)
						<< ": id="
						<< std::get<2>(rank)
						<< endl;
				}
			//}
			auto finish = chrono::high_resolution_clock::now();
			cout << os.str();
			cout << "Iterate MinMax " << findCount << ": " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";
		}



		cout << "===================== " << endl;



		typedef MinMaxTree<128, false> MMSortDesc;

		MMSortDesc minmaxd;

		start = chrono::high_resolution_clock::now();
		{
			//unsigned int insertTotal = 100;
			int j = 0;
			for (int i = 0; i < insertTotal; i++) {
			//for (int i = insertTotal; i > 0; i--) {
				//minmax.insert(i, i);
				minmaxd.insert(inserted[i], i);
			}
			auto finish = chrono::high_resolution_clock::now();
			cout << "Insert " << insertTotal << " : " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";

		}

		//minmaxd.display();

		start = chrono::high_resolution_clock::now();
		{
			int findValue = insertTotal;
			int findCount = 10;

			cout << "Show top ranks, higher is better \n";

			//for (int j = 0; j < 1; j++) {
				std::vector<std::tuple<unsigned int, unsigned long long, unsigned long long>> ranks = minmaxd.revrange(findValue, findCount);
				for (std::tuple<unsigned int, unsigned long long, unsigned long long> rank : ranks) {
					cout << "Rank ["
						<< std::get<0>(rank)
						<< "]: score="
						<< std::get<1>(rank)
						<< ": id="
						<< std::get<2>(rank)
						<< endl;
				}
			//}
			auto finish = chrono::high_resolution_clock::now();
			cout << "Iterate MinMax " << findCount << ": " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";
		}
	}
	
	Engine* engine = new Engine();
	CommandLoop cl;
	cl.Begin(engine);

	return 0;
}

