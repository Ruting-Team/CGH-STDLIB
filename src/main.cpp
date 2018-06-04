#include <iostream>
#include "Parser.hpp"
#include "FA.hpp"

using namespace cgh;

void test(string fileName) {
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

}

int main(int argc, char const *argv[])
{
	cout << "Hello, CGH!" << endl;
    string fileName1 = "";
    string fileName2 = "";
    string op = "";
	if (argc > 1) {
        fileName1 = argv[1];
        fileName2 = argv[2];
        op = argv[3];
	}
    Parser parser1(fileName1);
    Parser parser2(fileName2);
    RawFaData data1 = parser1.parse();
    RawFaData data2 = parser2.parse();
    NFA nfa1(data1);
    NFA nfa2(data2);
    nfa1.output();
    cout<<endl;
    nfa2.output();
    cout<<endl; 
    if(op == "-i") (nfa1 & nfa2).output();
    else if(op == "-u") (nfa1 | nfa2).output();
    else if(op == "-c")
    {
        (!nfa1).output();
        cout<<endl;
        (!nfa1).output();
    }

	
	
	
	
	return 0;
}
