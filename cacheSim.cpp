#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include "memory.h"

using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;

int main(int argc, char **argv) {
	Memory memory;
	Write_Policy write_policy;

	if (argc < 19) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	// Get input arguments

	// File
	// Assuming it is the first argument
	char* fileString = argv[1];
	ifstream file(fileString); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
			L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

	for (int i = 2; i < 19; i += 2) {
		string s(argv[i]);
		if (s == "--mem-cyc") {
			MemCyc = atoi(argv[i + 1]);
		} else if (s == "--bsize") {
			BSize = atoi(argv[i + 1]);
		} else if (s == "--l1-size") {
			L1Size = atoi(argv[i + 1]);
		} else if (s == "--l2-size") {
			L2Size = atoi(argv[i + 1]);
		} else if (s == "--l1-cyc") {
			L1Cyc = atoi(argv[i + 1]);
		} else if (s == "--l2-cyc") {
			L2Cyc = atoi(argv[i + 1]);
		} else if (s == "--l1-assoc") {
			L1Assoc = atoi(argv[i + 1]);
		} else if (s == "--l2-assoc") {
			L2Assoc = atoi(argv[i + 1]);
		} else if (s == "--wr-alloc") {
			WrAlloc = atoi(argv[i + 1]);
		} else {
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}

	/* initialize memory stuct */
	write_policy = (WrAlloc)? WRITE_ALLOC : NO_WRITE_ALLOC;
	memory = CreateMemory(L1Size, L2Size, BSize, L1Assoc, L2Assoc,
				L1Cyc, L2Cyc, MemCyc, write_policy);

	if (!memory)
	    return -1;

	while (getline(file, line)) {

		stringstream ss(line);
		string address;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address)) {
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}

		if (operation != 'r' && operation != 'w') {
			cout << "Invalid operation " << operation << endl;
			return -2;
		}

		string cutAddress = address.substr(2); // Removing the "0x" part of the address

		unsigned long int num = 0;
		num = strtoul(cutAddress.c_str(), NULL, 16);

		Operation op = (operation == 'r') ? OP_READ : OP_WRITE;
		CacheOperation(memory, op, num);

	}

	double L1MissRate = returnMissRate(memory,1);
	double L2MissRate = returnMissRate(memory,2);
	double avgAccTime = returnAvgAccTime(memory);

	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	DestroyMemory(memory);

	return 0;
}
