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
    string str = "";
	if (argc > 1) {
        fileName1 = argv[1];
        fileName2 = argv[2];
        op = argv[3];
        str = argv[4];
	}
    Word word;
    for(int i = 0; i < str.size(); i++)
        word.push_back((int)(str[i] - '0'));
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
    if(op == "-i")
    {
        DFA *dfa = dynamic_cast<DFA*>(&(nfa1 & nfa2));
        dfa->removeDeadState();
        dfa->output();
        
    }
    else if(op == "-d")
    {
        nfa1.determine().output();
        cout<<endl;
        nfa2.determine().output();
    }
    else if(op == "-u")
    {
        DFA *dfa = dynamic_cast<DFA*>(&(nfa1 | nfa2).determine());
        dfa->removeDeadState();
        dfa->output();
        cout<< (nfa1 <= (*dfa)) <<endl;
        cout<< (nfa2 <= (*dfa)) <<endl;
    }
    else if(op == "-cat") nfa1.concat(nfa2).determine().output();
    else if(op == "-com")
    {
        (!nfa1).output();
        cout<<endl;
        (!nfa1).output();
    }
    else if(op == "-e") cout<< (nfa1 == nfa2)<<endl;
    else if(op == "-s") cout<< (nfa1 <= nfa2)<<endl;
    else if(op == "-r")
    {
        cout<< (nfa1.determine().isReachable(word))<<endl;
        cout<< (nfa2.determine().isReachable(word))<<endl;
    }

	
	
	
	
	return 0;
}
