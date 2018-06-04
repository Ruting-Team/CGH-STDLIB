/*****************************************************************************/
/*  FA.cpp                                                                   */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/4/8.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#include "FA.hpp"
using namespace cgh;

bool FA::hasFinalState(const StateSet &stateSet)const
{
    StateSetConstIter iter;
    for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
        if((*iter)->isFinal()) return true;
    return false;
}

void FA::clearFinalStateSet()
{
    for(StateSetIter iter = finalStateSet.begin(); iter != finalStateSet.end(); iter++)
        (*iter)->setFinalFlag(0);
    finalStateSet.clear();
}


bool FA::operator ==(const FA& fa)
{
    DFA* cDFA = dynamic_cast<DFA*>(&(!(const_cast<FA&>(fa))));
    DFA* iDFA = dynamic_cast<DFA*>(&(*this & (*cDFA)));
    if(!iDFA->isEmpty())
    {
        delete cDFA;
        delete iDFA;
        return false;
    }
    cDFA = dynamic_cast<DFA*>(&(!(*this)));
    iDFA = dynamic_cast<DFA*>(&(*cDFA & fa));
    if(!iDFA->isEmpty())
    {
        delete cDFA;
        delete iDFA;
        return false;
    }
    delete cDFA;
    delete iDFA;
    return true;
}

bool FA::operator <=(const FA& fa)
{
    DFA* cDFA = dynamic_cast<DFA*>(&(!(const_cast<FA&>(fa))));
    DFA* iDFA = dynamic_cast<DFA*>(&(*this & (*cDFA)));
    if(!iDFA->isEmpty())
    {
        delete cDFA;
        delete iDFA;
        return false;
    }
    delete cDFA;
    delete iDFA;
    return true;
}

/*******************************************************************/
/*                                                                 */
/*  FA::mutipleIntersectionAndDeterminEmptiness                    */
/*                                                                 */
/*******************************************************************/
//void FA::getSMV(FASet &faset, vector<vector<vector<string> > >& SMV, int offset)
//{
//    vector<vector<string> > VAR;
//    vector<vector<string> > ASSIGH_init;
//    vector<vector<string> > ASSIGH_next;
//    vector<vector<string> > ASSIGH_init_next;
//    vector<vector<string> > INVARSPEC;
//    vector<vector<string> > smv;
//    int id = 0;
//    for(FASetIter iter = faset.begin(); iter != faset.end(); iter++)
//    {
//        smv.clear();
//        (*iter)->getSMV(smv, offset + id++);
//        VAR.push_back(smv[0]);
//        ASSIGH_init.push_back(smv[1]);
//        ASSIGH_init_next.push_back(smv[2]);
//        ASSIGH_next.push_back(smv[3]);
//        INVARSPEC.push_back(smv[4]);
//    }
//    string INVARSPECstr = "(";
//    for(int i = 0; i < INVARSPEC.size(); i++)
//        INVARSPECstr += " " + INVARSPEC[i][0] + " &";
//    INVARSPECstr.pop_back();
//    INVARSPECstr += ")";
//    vector<string> INVAR;
//    INVAR.push_back(INVARSPECstr);
//    INVARSPEC.clear();
//    INVARSPEC.push_back(INVAR);
//    SMV.clear();
//    SMV.push_back(VAR);
//    SMV.push_back(ASSIGH_init);
//    SMV.push_back(ASSIGH_init_next);
//    SMV.push_back(ASSIGH_next);
//    SMV.push_back(INVARSPEC);
//}
//
//void FA::getSMV(list<FASet> &fasetList, vector<vector<string> >& SMV)
//{
//    vector<string> VAR;
//    vector<string> ASSIGH_init;
//    vector<string> ASSIGH_next;
//    vector<string> ASSIGH_init_next;
//    vector<string> INVARSPEC;
//    vector<vector<vector<string> > > smv;
//    int offset = 0;
//    for(list<FASet>::iterator iter = fasetList.begin(); iter != fasetList.end(); iter++)
//    {
//        smv.clear();
//        getSMV(*iter, smv, offset);
//        offset = (int)(*iter).size();
//        for(int i = 0; i < smv[0].size(); i++)
//        {
//            VAR.insert(VAR.end(), smv[0][i].begin(), smv[0][i].end());
//            ASSIGH_init.insert(ASSIGH_init.end(), smv[1][i].begin(), smv[1][i].end());
//            if(iter != fasetList.begin())
//            {
//                for(int j = 1; j < smv[2][i].size(); j++)
//                    smv[2][i][j] = INVARSPEC.back() + " & " + smv[2][i][j];
//                smv[2][i][0] = "!" + INVARSPEC.back() + " & " + smv[2][i][0];
//            }
//            else
//                smv[2][i].erase(smv[2][i].begin());
//            smv[3][i].insert(++smv[3][i].begin(), smv[2][i].begin(), smv[2][i].end());
//            ASSIGH_next.insert(ASSIGH_next.end(), smv[3][i].begin(), smv[3][i].end());
//        }
//        INVARSPEC.insert(INVARSPEC.end(), smv[4][0][0]);
//    }
//    string INVARSPECstr = "!" + INVARSPEC.back();
//    INVARSPEC.clear();
//    INVARSPEC.push_back(INVARSPECstr);
//    SMV.clear();
//    SMV.push_back(VAR);
//    SMV.push_back(ASSIGH_init);
//    SMV.push_back(ASSIGH_next);
//    SMV.push_back(INVARSPEC);
//    cout << "MODULE main" << endl;
//    cout << "VAR" << endl;
//    for(int i = 0; i < VAR.size(); i++)
//        cout << VAR[i] << endl;
//    cout << "ASSIGN" << endl;
//    for(int i = 0; i < ASSIGH_init.size(); i++)
//        cout << ASSIGH_init[i] << endl;
//    for(int i = 0; i < ASSIGH_next.size(); i++)
//        cout << ASSIGH_next[i] << endl;
//    cout << "INVARSPEC" << endl;
//    cout << INVARSPECstr << endl;
//}
//
//
//bool FA::multiIntersectionAndDeterminEmptiness(FASet &faset, Alphabet &charSet)
//{
//    int num = -1;
//    vector<string> VAR;
//    vector<string> ASSIGH_init;
//    vector<string> ASSIGH_next;
//    vector<string> INVARSPEC;
//    string endStr = "esac ;";
//    for(FASetIter iter = faset.begin(); iter != faset.end(); iter++)
//    {
//        StateSet stateset = (*iter)->stateSet;
//        string iniID = to_string((*iter)->initialState->getID());
//        string faStr = "state" + to_string(++num);
//        string trapStr = " p" + to_string(num);
//        string elseStr = "TRUE : " + trapStr + ";";
//        string stateSetString = faStr + " : {";
//        string iniStateSetString = "init(" + faStr + ") := s" + iniID + ";";
//        string nextString = "next(" + faStr + ") := case";
//        string finStr = "(";
//        ASSIGH_next.push_back(nextString);
//        for(StateSetIter sIter = stateset.begin(); sIter != stateset.end(); sIter++)
//        {
//            string stateStr = " s" + to_string((*sIter)->getID());
//            stateSetString += stateStr + ",";
//            vector<string> strVec = (*sIter)->getSMV(num);
//            ASSIGH_next.insert(ASSIGH_next.end(), strVec.begin(), strVec.end());
//            if((*sIter)->isFinal())
//                finStr += " " + faStr + " = " + stateStr + " |";
//        }
//        finStr.pop_back();
//        finStr += ")";
//        stateSetString += trapStr + "};";
//        VAR.push_back(stateSetString);
//        ASSIGH_init.push_back(iniStateSetString);
//        ASSIGH_next.push_back(elseStr);
//        ASSIGH_next.push_back(endStr);
//        INVARSPEC.push_back(finStr);
//    }
//    for(AlphabetIter iter = charSet.begin(); iter != charSet.end(); iter++)
//    {
//        string charString = "a" + to_string(*iter) + " : boolean;";
//        VAR.push_back(charString);
//    }
//    string INVARSPECstr = "!(";
//    for(int i = 0; i < INVARSPEC.size(); i++)
//        INVARSPECstr += " " + INVARSPEC[i] + " &";
//    INVARSPECstr.pop_back();
//    INVARSPECstr += ");";
//    ofstream outFile;
//    outFile.open("/Users/iEric/Downloads/tools/nuXmv-1.1.1-Darwin/bin/temp.smv");
//    outFile << "MODULE main" << endl;
//    outFile << "VAR" << endl;
//    for(int i = 0; i < VAR.size(); i++)
//        outFile << VAR[i] << endl;
//    outFile << "ASSIGN" << endl;
//    for(int i = 0; i < ASSIGH_init.size(); i++)
//        outFile << ASSIGH_init[i] << endl;
//    for(int i = 0; i < ASSIGH_next.size(); i++)
//        outFile << ASSIGH_next[i] << endl;
//    outFile << "INVARSPEC" << endl;
//    outFile << INVARSPECstr << endl;
//    outFile.close();
////    FILE* pipe = popen("/Downloads/tools/nuXmv-1.1.1-Darwin/bin/nuXmv -old nuXmv-1.1.1-Darwin/bin/temp.smv", "r");
////    if (!pipe) return "ERROR";
////    char buffer[128];
////    std::string result = "";
////    while(!feof(pipe)) {
////        if(fgets(buffer, 128, pipe) != NULL)
////            result += buffer;
////    }
////    pclose(pipe);
////    cout<<result;
//    return true;
//}

//bool FA::multiIntersectionAndDeterminEmptiness(list<FASet> &fasetList, FASet &faset, Alphabet &charSet)
//{
//    list<FASet>::iterator iter;
//    vector<string> VAR;
//    vector<string> ASSIGH_init;
//    vector<string> ASSIGH_init_next;
//    vector<string> ASSIGH_next;
//    vector<string> INVARSPEC;
//    for(iter = fasetList.begin(); iter != fasetList.end(); iter++)
//    {
//        vector<vector<string> > SMVVector = getSMV(*iter);
//        for(int i = 0; i < SMVVector[2].size(); i++)
//            if(INVARSPEC.size() > 0)
//                SMVVector[2][i] = INVARSPEC.back() + " & " + SMVVector[2][i];
//        SMVVector[3].insert(++SMVVector[3].begin(), SMVVector[2].begin(), SMVVector[2].end());
//        VAR.insert(VAR.end(), SMVVector[0].begin(), SMVVector[0].end());
//        ASSIGH_init.insert(ASSIGH_init.end(), SMVVector[1].begin(), SMVVector[1].end());
//        ASSIGH_next.insert(ASSIGH_next.end(), SMVVector[3].begin(), SMVVector[3].end());
//        INVARSPEC.insert(INVARSPEC.end(), SMVVector[4].begin(), SMVVector[4].end());
//    }
//    cout << "MODULE main" << endl;
//    cout << "VAR" << endl;
//    for(int i = 0; i < VAR.size(); i++)
//        cout << VAR[i] << endl;
//    cout << "ASSIGN" << endl;
//    for(int i = 0; i < ASSIGH_init.size(); i++)
//        cout << ASSIGH_init[i] << endl;
//    for(int i = 0; i < ASSIGH_next.size(); i++)
//        cout << ASSIGH_next[i] << endl;
//    cout << "INVARSPEC" << endl;
//    cout << INVARSPECstr << endl;
//    return 1;
//}


/*******************************************************************/
/*                                                                 */
/*  FA::isEmpty                                                    */
/*                                                                 */
/*******************************************************************/
bool FA::isEmpty()
{
    if(!initialState) return true;
    if(!isReachable()) removeUnreachableState();
    if(finalStateSet.size() == 0) return true;
    return false;
}
/*******************************************************************/
/*                                                                 */
/*  FA::makeFA                                                     */
/*                                                                 */
/*******************************************************************/
FA& FA::EmptyFA()
{
    DFA* dfa = new DFA();
    return *dfa;
}
FA& FA::CompleteFA(Alphabet charSet)
{
    DFA* dfa = new DFA();
    DFAState* iniState = dfa->mkDFAFinalState();
    dfa->initialState = iniState;
    dfa->setAlphabet(charSet);
    AlphabetIter iter;
    for(iter = charSet.begin(); iter != charSet.end(); iter++)
        iniState->addDFATrans(*iter, iniState);
    return *dfa;
}
FA& FA::SigmaStarFA(Alphabet charSet)
{
    DFA* dfa = new DFA();
    DFAState* iniState = dfa->mkDFAInitialState();
    dfa->addFinalState(iniState);
    dfa->setAlphabet(charSet);
    AlphabetIter iter;
    for(iter = charSet.begin(); iter != charSet.end(); iter++)
        iniState->addDFATrans(*iter, iniState);
    return *dfa;
}

/*******************************************************************/
/*                                                                 */
/*  FA::getSMV                                                    */
/*                                                                 */
/*******************************************************************/

//void FA::getSMV(vector<vector<string> >& SMV, int id)
//{
//    vector<string> VAR;
//    vector<string> ASSIGH_init;
//    vector<string> ASSIGH_init_next;
//    vector<string> ASSIGH_next;
//    vector<string> INVARSPEC;
//    string endStr = "esac ;";
//    string iniID = to_string(initialState->getID());
//    string faStr = "state" + to_string(id);
//    string trapStr = " p" + to_string(id);
//    string elseStr = "TRUE : " + trapStr + ";";
//    string stateSetString = faStr + " : {";
//    string iniStateSetString = "init(" + faStr + ") := s" + iniID + ";";
//    string nextString = "next(" + faStr + ") := case";
//    string ini2ini = faStr + " = s" + iniID + " : s" + iniID + ";";
//    string finStr = "(";
//    ASSIGH_next.push_back(nextString);
//    ASSIGH_init_next.push_back(ini2ini);
//    for(StateSetIter sIter = stateSet.begin(); sIter != stateSet.end(); sIter++)
//    {
//        string stateStr = " s" + to_string((*sIter)->getID());
//        stateSetString += stateStr + ",";
//        vector<string> strVec = (*sIter)->getSMV((int)id);
//        if(*sIter != initialState)
//            ASSIGH_next.insert(ASSIGH_next.end(), strVec.begin(), strVec.end());
//        else
//            ASSIGH_init_next.insert(ASSIGH_init_next.end(), strVec.begin(), strVec.end());
//        if((*sIter)->isFinal())
//            finStr += " " + faStr + " = " + stateStr + " |";
//    }
//    finStr.pop_back();
//    finStr += ")";
//    stateSetString += trapStr + "};";
//    VAR.push_back(stateSetString);
//    ASSIGH_init.push_back(iniStateSetString);
//    ASSIGH_next.push_back(elseStr);
//    ASSIGH_next.push_back(endStr);
//    INVARSPEC.push_back(finStr);
//    SMV.push_back(VAR);
//    SMV.push_back(ASSIGH_init);
//    SMV.push_back(ASSIGH_init_next);
//    SMV.push_back(ASSIGH_next);
//    SMV.push_back(INVARSPEC);
//}



