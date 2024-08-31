// MovStat.cpp : Defines the entry point for the application.
//


#include <cmdline/CommandLoop.h>
#include "db/Engine.h"
#include <string>
#include <memory>
#include <format>
#include <algorithm>
#include <random>

#include "db/datastructures/HashTree.h"
#include "db/datastructures/Base62.h"
#include "db/datastructures/BTree.h"

#include "db/datastructures/MinMaxSort.h"


using namespace std;
//#include "fc/btree.h"

struct BTData {
	int key;
	std::string name;
};


unsigned int totalTreeNodes = 0;
int main()
{
	cout << "LeStatDB v1.0 Started built using c++ " << __cplusplus << endl;


	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 1000000);

	//BTree<BTData> *bt = new BTree<BTData>(256);
	//frozenca::BTreeSet<std::int64_t, 64> bt;// = new frozenca::BTreeMap<int, BTData, 128>();
	{
		auto start = chrono::high_resolution_clock::now();

		//std::random_device rd;     // Only used once to initialise (seed) engine
		//std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
		//std::uniform_int_distribution<int> uni(1, 10000000); // Guaranteed unbiased


		typedef MinMaxSort<unsigned int, int, 256> MMSort;

		MMSort minmax(true);

		//unsigned int firstValue = 100000;
		unsigned int score = 1;
		for (int i = 1; i <= 1000000; i++) {
			/*if (i == 1) {
			firstValue = dist6(rng);
			minmax.insert(firstValue, i);
			}
			else {*/
			if (i % 2 == 0) {
				score++;
			}
				//minmax.insert((unsigned int)dist6(rng), i);
			//}
			minmax.insert(score, i);
		}

		auto finish = chrono::high_resolution_clock::now();
		cout << "Insert 10,000,000 MinMax : " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";

		start = chrono::high_resolution_clock::now();

		int findValue = 50000;
		int findCount = 100;

		for (int j=0; j < 1000; j++) {
			std::vector<std::tuple<unsigned int, unsigned int,int>> ranks = minmax.range(findValue, findCount);

			for (std::tuple<unsigned int, unsigned int,int> rank : ranks) {
				/*cout << "Rank [" 
					<< std::get<0>(rank) 
					<< "]: score="
					<< std::get<1>(rank)
					<< ": value="
					<< std::get<2>(rank)
					<< endl;*/
			}

		}

		//minmax.traverse(findValue, [](MMSort::MMTreeNode* element) -> void {
		//	totalTreeNodes++;// += element->node->size;
		//	//std::cout << element->node->size << ' ';
		//});

		//std::cout << "Total Tree Nodes: " << totalTreeNodes << endl;
	

		finish = chrono::high_resolution_clock::now();
		cout << "Iterate MinMax " << findCount << ": " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";


		start = chrono::high_resolution_clock::now();
		int count = 0;

		for (int i = 0; i < 100000000; i++) {
			count++;
		}

		finish = chrono::high_resolution_clock::now();
		cout << "1M loop " << ": " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";

	}
	

	Engine* engine = new Engine();
	CommandLoop cl;
	cl.Begin(engine);

	return 0;
}

