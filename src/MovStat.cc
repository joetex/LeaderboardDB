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

//#include "db/datastructures/MinMaxSort.h"
#include "db/datastructures/RecursiveMinMax.h"

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

		std::random_device rd;     // Only used once to initialise (seed) engine
		std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
		std::uniform_int_distribution<int> uni(1, 10000000); // Guaranteed unbiased


		//typedef MinMaxSort<unsigned int, int, 128> MMSort;

		typedef RecursiveMinMax<unsigned int, int, 4, true> MMSort;

		MMSort minmax;

		//unsigned int firstValue = 100000;
		//unsigned int score = 10000;
		unsigned int insertTotal = 20;
		for (int i = 1; i <= insertTotal; i++) {
			/*if (i == 1) {
			firstValue = dist6(rng);
			minmax.insert(firstValue, i);
			}
			else {*/
			//if (i % 2 == 0) {
				//score++;
			//}
				//minmax.insert((unsigned int)dist6(rng), i);
			//}
			//unsigned int rkey = dist6(rng);
			cout << "-------------------- " << i << ":"  << " ----- " << endl;
			minmax.insert(i, i);
			minmax.display(nullptr);
		}

		/*for (int i = 1; i <= 5; i++) {
			cout << "-------------------- " << insertTotal+i << " ----- " << endl;
			minmax.insert(5, 5);
			minmax.display(nullptr);
		}*/

		auto finish = chrono::high_resolution_clock::now();
		cout << "Insert " << insertTotal << " : " << chrono::duration_cast<chrono::milliseconds>(finish - start).count() << "ms\n";

		start = chrono::high_resolution_clock::now();

		int findValue = 16000;
		int findCount = 10;

		/*MMSort::MMNode* bestNode = minmax.searchBestNode(1);
		if (bestNode) {
			for (int i = 0; i < bestNode->size; i++) {
				MMSort::MMKey* kv = bestNode->data[i];
				if (kv) {
					cout << "Key [" << kv->key << "] = " << kv->value << endl;
				}
			}
		}*/

		MMSort::MMNode* root = minmax.root;
		if (root) {
			minmax.display(nullptr);
		}

		//for (int j=0; j < 1000; j++) {
		std::vector<std::tuple<unsigned int, unsigned int, int>> ranks;// = minmax.range(findValue, findCount);

			for (std::tuple<unsigned int, unsigned int,int> rank : ranks) {
				cout << "Rank [" 
					<< std::get<0>(rank) 
					<< "]: score="
					<< std::get<1>(rank)
					<< ": value="
					<< std::get<2>(rank)
					<< endl;
			}

		//}

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

