#include <iostream>
#include "Parser.hpp"

using namespace cgh;

int main(int argc, char const *argv[])
{
	cout << "Hello, CGH!" << endl;
	string fileName = "mini.fa";
	if (argc > 1) {
		fileName = argv[1];
	}
	Parser parser(fileName);
	RawFaData data = parser.parse();
	cout << "type: " << data.type << endl;
	
	cout << "states: " << data.stateNumber << endl;
	cout << "initial: " << data.initialState << endl;
	cout << "final: ";
	for (int final: data.finalStates) {
		cout << final << " ";
	}
	cout << endl;
	if (data.type == "int") {
		cout << "alphabet: ";
		RawFaDataWithInt* rawIntData = dynamic_cast<RawFaDataWithInt*>(data.alphabetAndTransitions);
		for (auto ch : rawIntData->alphabet) {
			cout << ch <<" ";
		}
		cout << endl;
		cout << "transitions" << endl;
		for (auto tr : rawIntData->transitions) {
			cout << get<0>(tr) << " " << get<1>(tr) << " " << get<2>(tr) << endl;
		}
		delete rawIntData;
	} else if (data.type == "char") {
		cout << "alphabet: ";
		RawFaDataWithChar* rawIntData = dynamic_cast<RawFaDataWithChar*>(data.alphabetAndTransitions);
		for (auto ch : rawIntData->alphabet) {
			cout << ch <<" ";
		}
		cout << endl;
		cout << "transitions" << endl;
		for (auto tr : rawIntData->transitions) {
			cout << get<0>(tr) << " " << get<1>(tr) << " " << get<2>(tr) << endl;
		}
		delete rawIntData;
	} else {
		cout << "alphabet: ";
		RawFaDataWithString* rawIntData = dynamic_cast<RawFaDataWithString*>(data.alphabetAndTransitions);
		for (auto ch : rawIntData->alphabet) {
			cout << ch <<" ";
		}
		cout << endl;
		cout << "transitions" << endl;
		for (auto tr : rawIntData->transitions) {
			cout << get<0>(tr) << " " << get<1>(tr) << " " << get<2>(tr) << endl;
		}
		delete rawIntData;
	}
	
	
	return 0;
}