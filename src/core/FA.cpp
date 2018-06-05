/*****************************************************************************/
/*  FA.cpp                                                                   */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/4/8.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#include "FA.hpp"
using namespace cgh;

bool FA::hasFinalState(const StateSet &stateSet)
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

/*******************************************************************/
/*                                                                 */
/*  FA::equal                                                      */
/*                                                                 */
/*******************************************************************/
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
/*******************************************************************/
/*                                                                 */
/*  FA::subset                                                      */
/*                                                                 */
/*******************************************************************/
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
/*  FA::multiIntersection                                          */
/*                                                                 */
/*******************************************************************/
void FA::getTransMapByStateSet(const StateSet& stateSet, NFATransMap& nfaTransMap)
{
    DFATransMap& transMap = dynamic_cast<DFAState*>(*stateSet.begin())->getDFATransMap();
    StateSet set;
    for(DFATransMapIter mapIter = transMap.begin(); mapIter != transMap.end(); mapIter++)
    {
        set.clear();
        set.insert(mapIter->second);
        for(StateSetConstIter iter = ++stateSet.begin(); iter != stateSet.end(); iter++)
        {
            DFAState* state = dynamic_cast<DFAState*>(*iter)->getTargetStateByChar(mapIter->first);
            if(!state) break;
            set.insert(state);
        }
        if(set.size() == stateSet.size()) nfaTransMap[mapIter->first] = set;
    }
}

void FA::makeDFATrans(DFAState* preState, SetMapping &setMapping, const NFATransMap &nfaTransMap, DFA* dfa)
{
    NFATransMap transMap;
    for(NFATransMapConstIter mapIter = nfaTransMap.begin(); mapIter != nfaTransMap.end(); mapIter++)
    {
        SetMapping::iterator setMapIter = setMapping.find(mapIter->second);
        DFAState* postState;
        if(setMapIter == setMapping.end())
        {
            transMap.clear();
            FA::getTransMapByStateSet(mapIter->second, transMap);
            if(FA::hasFinalState(mapIter->second)) postState = dfa->mkDFAFinalState();
            else postState = dfa->mkDFAState();
            setMapping[mapIter->second] = postState;
            FA::makeDFATrans(postState, setMapping, transMap, dfa);
        }
        else postState = dynamic_cast<DFAState*>(setMapping[mapIter->second]);
        preState->addDFATrans(mapIter->first, postState);
    }
}
FA &FA::multiIntersection(const FASet& faSet)//todo
{
    FASet tempFASet;
    StateSet set;
    bool f = true;
    for(FASetConstIter iter = faSet.begin(); iter != faSet.end(); iter++)
    {
        if(!(*iter)->getInitialState()) return FA::EmptyFA();
        State* iniState = NULL;
        if(!(*iter)->isDeterminate())
        {
            DFA *tempDFA = &dynamic_cast<const NFA*>(*iter)->determine();
            tempFASet.insert(tempDFA);
            iniState = tempDFA->initialState;
        }
        else iniState = (*iter)->initialState;
        f &= iniState->isFinal();
        set.insert(iniState);
    }
    DFA *dfa = new DFA();
    DFAState* iniState = dfa->mkDFAInitialState();
    if(f) dfa->addFinalState(iniState);
    SetMapping setMapping;
    setMapping[set] = iniState;
    NFATransMap nfaTransMap;
    FA::getTransMapByStateSet(set, nfaTransMap);
    FA::makeDFATrans(iniState, setMapping, nfaTransMap, dfa);
    for(FASetIter iter = tempFASet.begin(); iter != tempFASet.end(); iter++)
        delete *iter;
    if(dfa->finalStateSet.size() == 0)
    {
        delete dfa;
        return dynamic_cast<DFA&>(FA::EmptyFA());
    }
    dfa->setReachableFlag(1);
    return *dfa;
}
/*******************************************************************/
/*                                                                 */
/*  FA::multiUnion                                                 */
/*                                                                 */
/*******************************************************************/

FA &FA::multiUnion(const FASet &faSet)
{
    NFA *nfa = new NFA();
    NFAState* iniState = nfa->mkNFAInitialState();
    State2Map state2Map;
    for(FASetConstIter iter = faSet.begin(); iter != faSet.end(); iter++)
    {
        if(!(*iter)->getInitialState()) continue;
        state2Map.clear();
        NFAState* state = nfa->mkNFAState();
        iniState->addEpsilonTrans(state);
        if((*iter)->initialState->isFinal()) nfa->addFinalState(state);
        state2Map[(*iter)->initialState] = state;
        if((*iter)->isDeterminate())
            nfa->makeCopyTransByDFA(dynamic_cast<DFAState*>((*iter)->initialState), state2Map);
        else
            nfa->makeCopyTransByNFA(dynamic_cast<NFAState*>((*iter)->initialState), state2Map);
    }
    return *nfa;
}

/*******************************************************************/
/*                                                                 */
/*  FA::multiConcatination                                         */
/*                                                                 */
/*******************************************************************/

FA &FA::multiConcatination(const FAList &faList)
{
    NFA *nfa = new NFA();
    NFAState* iniState = nfa->mkNFAInitialState();
    State2Map state2Map;
    StateSet fStateSet;
    fStateSet.insert(iniState);
    for(FAListConstIter iter = faList.begin(); iter != faList.end(); iter++)
    {
        if(!(*iter)->getInitialState()) continue;
        state2Map.clear();
        NFAState* state = nfa->mkNFAState();
        for(StateSetIter sIter = fStateSet.begin(); sIter != fStateSet.end(); sIter++)
            dynamic_cast<NFAState*>(*sIter)->addEpsilonTrans(state);
        fStateSet.clear();
        nfa->clearFinalStateSet();
        if((*iter)->initialState->isFinal()) nfa->addFinalState(state);
        state2Map[(*iter)->initialState] = state;
        if((*iter)->isDeterminate())
            nfa->makeCopyTransByDFA(dynamic_cast<DFAState*>((*iter)->initialState), state2Map);
        else
            nfa->makeCopyTransByNFA(dynamic_cast<NFAState*>((*iter)->initialState), state2Map);
        fStateSet.insert(nfa->finalStateSet.begin(), nfa->finalStateSet.end());
    }
    return *nfa;
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



