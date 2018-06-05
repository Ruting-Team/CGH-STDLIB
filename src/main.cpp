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
    if(argc > 1)
    {
        string fileName1 = "";
        string fileName2 = "";
        string op = argv[1];
        if(argc == 3)
        {
            fileName1 = argv[2];
            Parser parser(fileName1);
            RawFaData *data = &parser.parse();
            NFA nfa(*data);
            cout<<"nfa:"<<endl;
            nfa.output();
            cout<<endl;
            if(op == "-d")
            {
                cout<<"determinate:"<<endl;
                NFA res(dynamic_cast<DFA&>(nfa.determine()));
                res.output();
                print(res, "res.dot");
            }
        }
        else if(argc == 4)
        {
            if(op != "-r")
            {
                fileName1 = argv[2];
                fileName2 = argv[3];
                Parser parser1(fileName1);
                Parser parser2(fileName2);
                RawFaData *data1 = &parser1.parse();
                RawFaData *data2 = &parser2.parse();
                NFA nfa1(*data1);
                NFA nfa2(*data2);
                cout<<"nfa1:"<<endl;
                nfa1.output();
                cout<<"nfa2:"<<endl;
                nfa2.output();
                cout<<endl;
                if(op == "-i")
                {
                    cout<<"intersection:"<<endl;
                    NFA res(dynamic_cast<DFA&>((nfa1 & nfa2)));
                    res.output();
                    if(!res.isEmpty())
                    print(res, "res.dot");
                }
                else if(op == "-u")
                {
                    cout<<"union:"<<endl;
                    NFA res(dynamic_cast<NFA&>((nfa1 | nfa2)).determine());
                    res.output();
                    if(!res.isEmpty())
                    print(res, "res.dot");
                }
                else if(op == "-cat")
                {
                    cout<<"concat:"<<endl;
                    NFA res(dynamic_cast<NFA&>((nfa1.concat(nfa2))).determine());
                    res.output();
                    if(!res.isEmpty())
                    print(res, "res.dot");
                }
            }
            else
            {
                fileName1 = argv[2];
                string str = argv[3];
                Word word;
                for(int i = 0; i < str.size(); i++)
                    word.push_back((int)(str[i] - '0'));
                Parser parser(fileName1);
                RawFaData *data = &parser.parse();
                NFA nfa(*data);
                cout<<"nfa:"<<endl;
                nfa.output();
                cout<<endl;
                cout<<"word:" + str<<endl;
                if(nfa.isReachable(word)) cout<<"Reachable"<<endl;
                else cout<<"Not Reachable"<<endl;
            }
        }
        else if(argc > 4)
        {
            if(op == "-mi")
            {
                FASet faSet;
                for(int i = 2; i < argc; i++)
                {
                    Parser parser(argv[i]);
                    RawFaData *data = &parser.parse();
                    NFA *nfa = new NFA(*data);
                    cout<<"nfa" + to_string(i - 1) + ":"<<endl;
                    nfa->output();
                    cout<<endl;
                    faSet.insert(nfa);
                }
                cout<<"multiIntersection:"<<endl;
                NFA res(dynamic_cast<DFA&>(FA::multiIntersection(faSet)));
                for(FASetIter iter = faSet.begin(); iter != faSet.end(); iter++)
                    delete dynamic_cast<NFA*>(*iter);
                res.output();
                if(!res.isEmpty())
                print(res, "res.dot");
            }
            else if(op == "-mu")
            {
                FASet faSet;
                for(int i = 2; i < argc; i++)
                {
                    Parser parser(argv[i]);
                    RawFaData *data = &parser.parse();
                    NFA *nfa = new NFA(*data);
                    cout<<"nfa" + to_string(i - 1) + ":"<<endl;
                    nfa->output();
                    cout<<endl;
                    faSet.insert(nfa);
                }
                cout<<"multiUnion:"<<endl;
                NFA res(dynamic_cast<NFA&>(FA::multiUnion(faSet)).determine());
                for(FASetIter iter = faSet.begin(); iter != faSet.end(); iter++)
                    delete dynamic_cast<NFA*>(*iter);
                res.output();
                if(!res.isEmpty())
                    print(res, "res.dot");
            }
            else if(op == "-mc")
            {
                FAList faList;
                for(int i = 2; i < argc; i++)
                {
                    Parser parser(argv[i]);
                    RawFaData *data = &parser.parse();
                    NFA *nfa = new NFA(*data);
                    cout<<"nfa" + to_string(i - 1) + ":"<<endl;
                    nfa->output();
                    cout<<endl;
                    faList.push_back(nfa);
                }
                cout<<"multiConcatination:"<<endl;
                NFA res(dynamic_cast<NFA&>(FA::multiConcatination(faList)).determine());
                for(FAListIter iter = faList.begin(); iter != faList.end(); iter++)
                    delete dynamic_cast<NFA*>(*iter);
                res.output();
                if(!res.isEmpty())
                    print(res, "res.dot");
            }
        }
    }
	return 0;
}
