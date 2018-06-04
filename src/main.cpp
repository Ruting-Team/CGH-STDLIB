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

void print(FA& fa, string filename) {

	ofstream f;
	f.open(filename);
	f << "digraph {\n";
	f << "rankdir=LR;\n";

	
	State* initialState = fa.getInitialState();
	// cout initial
	f << "Q"<<initialState->getID() << "[color=blue];\n";
	f << "node [shape=doublecircle];\n";



	// cout final states
	StateSet& finalStateSet = fa.getFinalStateSet();
	for (StateSetConstIter iter = finalStateSet.begin(); iter != finalStateSet.end(); iter++) {
		f << "Q" << (*iter)->getID() << " ";
	}
	f << ";\n";
	

	

	f << "node [shape=circle];\n";



	// cout trisitions
	StateSet& stateSet = fa.getStateSet();
	for(StateSetConstIter iter = stateSet.begin(); iter != stateSet.end(); iter++)
    {
    	NFATransMap& transMap = dynamic_cast<NFAState*>(*iter)->getNFATransMap();
    	ID id = (*iter)->getID();
    	for (NFATransMapIter iter = transMap.begin(); iter != transMap.end(); iter++) {
    		for (StateSetIter iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++) {
    			f << "Q" << id <<  " -> " << "Q" << (*iter1)->getID() << "[label=\"" << iter->first <<"\"];\n";
    		}
    	}
    }
	f <<"}\n";
	f.close();
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

    print(nfa1, "nfa1.dot");
    print (nfa2, "nfa2.dot");

    if (op == "-u") {

    	FA& res = (nfa1 | nfa2).determine().nondetermine();
    	
    	print(res, "res.dot");
    }
    

	
	
	
	
	return 0;
}
