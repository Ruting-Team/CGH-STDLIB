/*****************************************************************************/
/*  FA.cpp                                                                   */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/4/8.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#include "FA.hpp"
using namespace cgh;
/*******************************************************************/
/*                                                                 */
/*  NFAState::addNFATrans                                          */
/*                                                                 */
/*******************************************************************/
bool NFAState::addNFATrans(Character character, NFAState *target)
{
    NFATransMapIter MapIt = nfaTransMap.find(character);
    if(MapIt == nfaTransMap.end())
    {
        StateSet stateSet;
        stateSet.insert(target);
        nfaTransMap[character] = stateSet;
        return true;
    }
    else
        return MapIt->second.insert(target).second;
}
bool NFAState::addEpsilonTrans(NFAState *target)
{
    return addNFATrans(EPSILON, target);
}
/*******************************************************************/
/*                                                                 */
/*  NFAState::delNFATrans                                          */
/*                                                                 */
/*******************************************************************/
bool NFAState::delNFATrans(Character character, NFAState *target)
{
    NFATransMapIter MapIt = nfaTransMap.find(character);
    if(MapIt == nfaTransMap.end())
        return false;
    else
        return MapIt->second.erase(target) < MapIt->second.size() ? 1 : 0;
}
bool NFAState::delNFATrans(Character character)
{
    NFATransMapIter MapIt = nfaTransMap.find(character);
    if(MapIt == nfaTransMap.end())
        return false;
    nfaTransMap.erase(MapIt);
    return true;
}
/*******************************************************************/
/*                                                                 */
/*  NFAState::getEpsilonClosure                                    */
/*                                                                 */
/*******************************************************************/
const StateSet NFAState::getEpsilonClosure()
{
    StateSet tempSet;
    StateSetIter iter;
    StateSet epsilonStateSet;
    NFATransMapIter mapIter = nfaTransMap.find(EPSILON);
    if(mapIter == nfaTransMap.end()) return epsilonStateSet;
    epsilonStateSet.insert(mapIter->second.begin(), mapIter->second.end());
    for(iter = epsilonStateSet.begin(); iter != epsilonStateSet.end(); iter++)
    {
        StateSet epsilonSet = dynamic_cast<NFAState*>(*iter)->getEpsilonClosure();
        tempSet.insert(epsilonSet.begin(), epsilonSet.end());
    }
    if(tempSet.size()>0) epsilonStateSet.insert(tempSet.begin(), tempSet.end());
    return epsilonStateSet;
}
/*******************************************************************/
/*                                                                 */
/*  NfaState::getTargetStateSet                                    */
/*                                                                 */
/*******************************************************************/
const StateSet NFAState::getTargetStateSet()
{
    StateSetIter iter;
    StateSet stateSet;
    NFATransMapIter miter;
    for(miter = nfaTransMap.begin(); miter != nfaTransMap.end(); miter++)
    {
        StateSet tempSet = getTargetStateSetByChar(miter->first);
        stateSet.insert(tempSet.begin(), tempSet.end());
    }
    return stateSet;
}
/*******************************************************************/
/*                                                                 */
/*  NfaState::getTargetStateSetByChar                              */
/*                                                                 */
/*******************************************************************/
const StateSet NFAState::getTargetStateSetByChar(Character character)
{
    if(character == EPSILON) return getEpsilonClosure();
    StateSetIter iter;
    StateSet tempSet;
    StateSet stateSet;
    StateSet epsilonClosure = getEpsilonClosure();
    NFATransMapIter mapIter = nfaTransMap.find(character);
    if(mapIter != nfaTransMap.end())
        stateSet.insert(mapIter->second.begin(), mapIter->second.end());
    for(iter = epsilonClosure.begin(); iter != epsilonClosure.end(); iter++)
    {
        StateSet tempSet = dynamic_cast<NFAState*>((*iter))->getTargetStateSetByChar(character);
        if(tempSet.size() > 0) stateSet.insert(tempSet.begin(), tempSet.end());
    }
    return stateSet;
}
/*******************************************************************/
/*                                                                 */
/*  NFAState::NFAMapIterator                                       */
/*                                                                 */
/*******************************************************************/
NFAMapIter NFAState::NFAMapIterator(Character c)
{
    NFATransMapIter iter = nfaTransMap.find(c);
    bool b = 1;
    if(iter == nfaTransMap.end()) b = 0;
    return NFAMapIter(iter, b);
}

/*******************************************************************/
/*                                                                 */
/*  NFAState::getSMV                                               */
/*                                                                 */
/*******************************************************************/
vector<string> NFAState::getSMV(int id)
{
    string faStr = "state" + to_string(id);
    vector<string> strVector;
    for(NFATransMapIter iter = nfaTransMap.begin(); iter != nfaTransMap.end(); iter++)
    {
        string str = faStr + " = s" + to_string(getID()) + " & a" + to_string(iter->first) + " = TRUE : s";
        StateSet set = iter->second;
        for(StateSetIter sIter = set.begin(); sIter != set.end(); sIter++)
        {
            str += to_string((*sIter)->getID()) + ";";
            strVector.push_back(str);
        }
    }
    return strVector;
}

/*******************************************************************/
/*                                                                 */
/*  DFAState::addDFATrans                                          */
/*                                                                 */
/*******************************************************************/
bool DFAState::addDFATrans(Character character, DFAState *target)
{
    if(dfaTransMap.find(character) != dfaTransMap.end())
        return false;
    dfaTransMap[character] = target;
    return true;
}
/*******************************************************************/
/*                                                                 */
/*  DfaState::delDFATrans                                          */
/*                                                                 */
/*******************************************************************/
bool DFAState::delDFATrans(Character character, DFAState *target)
{
    DFATransMapIter MapIt = dfaTransMap.find(character);
    if(MapIt != dfaTransMap.end() && MapIt->second == target)
    {
        dfaTransMap.erase(MapIt);
        return true;
    }
    return false;
}

bool DFAState::delDFATrans(State *target)
{
    DFATransMapIter iter;
    AlphabetIter aIter;
    Alphabet charSet;
    for(iter = dfaTransMap.begin(); iter != dfaTransMap.end(); iter++)
        if(iter->second == target) charSet.insert(iter->first);
    for(aIter = charSet.begin(); aIter != charSet.end(); aIter++)
        dfaTransMap.erase(*aIter);
    return true;
}

bool DFAState::delDFATrans(Character character)
{
    DFATransMapIter MapIt = dfaTransMap.find(character);
    if(MapIt == dfaTransMap.end()) return false;
    dfaTransMap.erase(MapIt);
    return true;
}
/*******************************************************************/
/*                                                                 */
/*  DFAState::getSMV                                               */
/*                                                                 */
/*******************************************************************/
vector<string> DFAState::getSMV(int id)
{
    vector<string> strVector;
    string faStr = "state" + to_string(id);
    for(DFATransMapIter iter = dfaTransMap.begin(); iter != dfaTransMap.end(); iter++)
    {
        string str = faStr + " = s" + to_string(getID()) + " & a" + to_string(iter->first) + " = TRUE : s" + to_string(iter->second->getID()) + ";";
        strVector.push_back(str);
    }
    return strVector;
}
/*******************************************************************/
/*                                                                 */
/*  FA::mutipleIntersectionAndDeterminEmptiness                    */
/*                                                                 */
/*******************************************************************/
bool FA::multiIntersectionAndDeterminEmptiness(FASet &faset, Alphabet &charSet)
{
    int num = -1;
    vector<string> VAR;
    vector<string> ASSIGH_init;
    vector<string> ASSIGH_next;
    vector<string> INVARSPEC;
    string endStr = "esac ;";
    for(FASetIter iter = faset.begin(); iter != faset.end(); iter++)
    {
        StateSet stateset = (*iter)->stateSet;
        string iniID = to_string((*iter)->initialState->getID());
        string faStr = "state" + to_string(++num);
        string trapStr = " p" + to_string(num);
        string elseStr = "TRUE : " + trapStr + ";";
        string stateSetString = faStr + " : {";
        string iniStateSetString = "init(" + faStr + ") := s" + iniID + ";";
        string nextString = "next(" + faStr + ") := case";
        string finStr = "(";
        ASSIGH_next.push_back(nextString);
        for(StateSetIter sIter = stateset.begin(); sIter != stateset.end(); sIter++)
        {
            string stateStr = " s" + to_string((*sIter)->getID());
            stateSetString += stateStr + ",";
            vector<string> strVec = (*sIter)->getSMV(num);
            ASSIGH_next.insert(ASSIGH_next.end(), strVec.begin(), strVec.end());
            if((*sIter)->isFinal())
                finStr += " " + faStr + " = " + stateStr + " |";
        }
        finStr.pop_back();
        finStr += ")";
        stateSetString += trapStr + "};";
        VAR.push_back(stateSetString);
        ASSIGH_init.push_back(iniStateSetString);
        ASSIGH_next.push_back(elseStr);
        ASSIGH_next.push_back(endStr);
        INVARSPEC.push_back(finStr);
    }
    for(AlphabetIter iter = charSet.begin(); iter != charSet.end(); iter++)
    {
        string charString = "a" + to_string(*iter) + " : boolean;";
        VAR.push_back(charString);
    }
    string INVARSPECstr = "!(";
    for(int i = 0; i < INVARSPEC.size(); i++)
        INVARSPECstr += " " + INVARSPEC[i] + " &";
    INVARSPECstr.pop_back();
    INVARSPECstr += ");";
    ofstream outFile;
    outFile.open("/Users/iEric/Downloads/tools/nuXmv-1.1.1-Darwin/bin/temp.smv");
    outFile << "MODULE main" << endl;
    outFile << "VAR" << endl;
    for(int i = 0; i < VAR.size(); i++)
        outFile << VAR[i] << endl;
    outFile << "ASSIGN" << endl;
    for(int i = 0; i < ASSIGH_init.size(); i++)
        outFile << ASSIGH_init[i] << endl;
    for(int i = 0; i < ASSIGH_next.size(); i++)
        outFile << ASSIGH_next[i] << endl;
    outFile << "INVARSPEC" << endl;
    outFile << INVARSPECstr << endl;
    outFile.close();
//    FILE* pipe = popen("/Downloads/tools/nuXmv-1.1.1-Darwin/bin/nuXmv -old nuXmv-1.1.1-Darwin/bin/temp.smv", "r");
//    if (!pipe) return "ERROR";
//    char buffer[128];
//    std::string result = "";
//    while(!feof(pipe)) {
//        if(fgets(buffer, 128, pipe) != NULL)
//            result += buffer;
//    }
//    pclose(pipe);
//    cout<<result;
    return true;
}
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
/*******************************************************************/
/*                                                                 */
/*  NFA::construction                                              */
/*                                                                 */
/*******************************************************************/
void NFA::makeCopyTransByDFA(cgh::DFAState *state, State2Map &state2map)
{
    DFATransMapIter mapIter;
    DFATransMap map = state->getDFATransMap();
    NFAState* preState = dynamic_cast<NFAState*>(state2map[state]);
    for(mapIter = map.begin(); mapIter != map.end(); mapIter++)
    {
        NFAState* postState;
        State2MapIter state2MapIter = state2map.find(mapIter->second);
        if(state2MapIter == state2map.end())
        {
            if(mapIter->second->isFinal()) postState = mkNFAFinalState();
            else postState = mkNFAState();
            state2map[mapIter->second] = postState;
            makeCopyTransByDFA(dynamic_cast<DFAState*>(mapIter->second), state2map);
        }
        else postState = dynamic_cast<NFAState*>(state2MapIter->second);
        preState->addNFATrans(mapIter->first, postState);
    }
}
void NFA::makeCopyTransByNFA(cgh::NFAState *state, State2Map &state2map)
{
    NFATransMapIter mapIter;
    NFATransMap map = state->getNFATransMap();
    NFAState* preState = dynamic_cast<NFAState*>(state2map[state]);
    for(mapIter = map.begin(); mapIter != map.end(); mapIter++)
    {
        NFAState* postState;
        StateSet set = mapIter->second;
        for(StateSetIter iter = set.begin(); iter != set.end(); iter++)
        {
            State2MapIter state2MapIter = state2map.find(*iter);
            if(state2MapIter == state2map.end())
            {
                if((*iter)->isFinal()) postState = mkNFAFinalState();
                else postState = mkNFAState();
                state2map[*iter] = postState;
                makeCopyTransByNFA(dynamic_cast<NFAState*>(*iter), state2map);
            }
            else postState = dynamic_cast<NFAState*>(state2MapIter->second);
            preState->addNFATrans(mapIter->first, postState);
        }
    }
}
NFA::NFA(const NFA& nfa)
{
    if(nfa.initialState)
    {
        setAlphabet(nfa.alphabet);
        NFAState* iniState = mkNFAInitialState();
        State2Map state2Map;
        state2Map[nfa.initialState] = iniState;
        makeCopyTransByNFA(dynamic_cast<NFAState*>(nfa.initialState), state2Map);
    }
}
NFA::NFA(DFA& dfa)
{
    if(dfa.initialState)
    {
        setAlphabet(dfa.alphabet);
        NFAState* iniState = mkNFAInitialState();
        State2Map state2Map;
        state2Map[dfa.initialState] = iniState;
        makeCopyTransByDFA(dynamic_cast<DFAState*>(dfa.initialState), state2Map);
    }
}
NFA::NFA(RawFaData& data)
{
    flag = 0;
    RawFaDataWithInt* rawdata = dynamic_cast<RawFaDataWithInt*>(data.alphabetAndTransitions);
    setAlphabet(rawdata->alphabet);
    int stateNum = data.stateNumber;
    vector<NFAState*> stateVector;
    for(int pos = 0; pos < stateNum; pos++)
    {
        if(pos == data.initialState) stateVector.push_back(mkNFAInitialState());
        else if(data.finalStates.find(pos) != data.finalStates.end()) stateVector.push_back(mkNFAFinalState());
        else stateVector.push_back(mkNFAState());
    }
    if(data.finalStates.find(data.initialState) != data.finalStates.end())
    {
        initialState->setFinalFlag(1);
        finalStateSet.insert(initialState);
    }
    vector<tuple<int, int, int> > transVector = rawdata->transitions;
    for(int i = 0; i < transVector.size(); i++)
    {
        int sourceState = get<0>(transVector[i]);
        int targetState = get<2>(transVector[i]);
        int character = get<1>(transVector[i]);
        stateVector[sourceState]->addNFATrans(character, stateVector[targetState]);
    }
}
/*******************************************************************/
/*                                                                 */
/*  NFA::mkNFAState                                                */
/*                                                                 */
/*******************************************************************/
NFAState *NFA::mkNFAState()
{
    NFAState *nfaState = new NFAState();
    stateSet.insert(nfaState);
    return nfaState;
}
NFAState *NFA::mkNFAState(ID i)
{
    NFAState *nfaState = new NFAState(i);
    stateSet.insert(nfaState);
    return nfaState;
}
NFAState *NFA::mkNFAInitialState()
{
    NFAState *nfaState = new NFAState();
    stateSet.insert(nfaState);
    initialState = nfaState;
    return nfaState;
}
NFAState *NFA::mkNFAInitialState(ID i)
{
    NFAState *nfaState = new NFAState(i);
    stateSet.insert(nfaState);
    initialState = nfaState;
    return nfaState;
}
NFAState *NFA::mkNFAFinalState()
{
    NFAState *nfaState = new NFAState();
    nfaState->setFinalFlag(1);
    stateSet.insert(nfaState);
    finalStateSet.insert(nfaState);
    return nfaState;
}
NFAState *NFA::mkNFAFinalState(ID i)
{
    NFAState *nfaState = new NFAState(i);
    nfaState->setFinalFlag(1);
    stateSet.insert(nfaState);
    finalStateSet.insert(nfaState);
    return nfaState;
}

/*******************************************************************/
/*                                                                 */
/*  DFAState::getTargetStateSet                                    */
/*                                                                 */
/*******************************************************************/
const StateSet DFAState::getTargetStateSet()
{
    StateSetIter iter;
    StateSet stateSet;
    DFATransMapIter miter;
    for(miter = dfaTransMap.begin(); miter != dfaTransMap.end(); miter++)
        stateSet.insert(miter->second);
    return stateSet;
}
/*******************************************************************/
/*                                                                 */
/*  DFAState::getTargetStateByChar                                 */
/*                                                                 */
/*******************************************************************/
DFAState* DFAState::getTargetStateByChar(Character character)
{
    DFATransMapIter iter = dfaTransMap.find(character);
    if(iter != dfaTransMap.end()) return dynamic_cast<DFAState*>(iter->second);
    DFAState* null = new DFAState();
    null->setNULL();
    return null;
}
/*******************************************************************/
/*                                                                 */
/*  NFA::intersection                                              */
/*                                                                 */
/*******************************************************************/
FA &NFA::operator &(const FA &fa)
{
    DFA dfa1 = toDFA();
    DFA dfa2 = const_cast<FA&>(fa).toDFA();
    return dfa1 & dfa2;
}
/*******************************************************************/
/*                                                                 */
/*  NFA::union                                                     */
/*                                                                 */
/*******************************************************************/
FA &NFA::operator |(const FA &fa)
{
    if(!initialState) return const_cast<FA&>(fa).toNFA();
    if(!fa.initialState) return *this;
    NFA *nfa = new NFA();
    nfa->setAlphabet(alphabet);
    nfa->addAlphabet(fa.alphabet);
    NFAState* iniState = nfa->mkNFAInitialState();
    NFAState* state1 = nfa->mkNFAState();
    NFAState* state2 = nfa->mkNFAState();
    State2Map state2Map1;
    State2Map state2Map2;
    NFA tempnfa = const_cast<FA&>(fa).toNFA();
    state2Map1[initialState] = state1;
    state2Map2[tempnfa.initialState] = state2;
    nfa->makeCopyTransByNFA(dynamic_cast<NFAState*>(initialState), state2Map1);
    nfa->makeCopyTransByNFA(dynamic_cast<NFAState*>(tempnfa.initialState), state2Map2);
    iniState->addEpsilonTrans(state1);
    iniState->addEpsilonTrans(state2);
    return *nfa;
}
/*******************************************************************/
/*                                                                 */
/*  NFA::concatination                                             */
/*                                                                 */
/*******************************************************************/
FA &NFA::operator +(const FA &fa)
{
    if(!initialState) return const_cast<FA&>(fa).toNFA();
    if(!fa.initialState) return *this;
    NFA *nfa = new NFA(*this);
    nfa->addAlphabet(fa.alphabet);
    StateSetIter iter;
    StateSet fStateSet;
    fStateSet.insert(nfa->finalStateSet.begin(), nfa->finalStateSet.end());
    nfa->finalStateSet.clear();
    NFAState* state = nfa->mkNFAState();
    State2Map state2Map;
    NFA tempnfa = const_cast<FA&>(fa).toNFA();
    state2Map[tempnfa.initialState] = state;
    nfa->makeCopyTransByNFA(dynamic_cast<NFAState*>(tempnfa.initialState), state2Map);
    for(iter = fStateSet.begin(); iter != fStateSet.end(); iter++)
    {
        (*iter)->setFinalFlag(0);
        dynamic_cast<NFAState*>(*iter)->addEpsilonTrans(state);
    }
    return *nfa;
}
/*******************************************************************/
/*                                                                 */
/*  NFA::minus                                                     */
/*                                                                 */
/*******************************************************************/
FA &NFA::operator -(const FA &fa)
{
    return (toDFA()) & !(const_cast<FA&>(fa));
}
/*******************************************************************/
/*                                                                 */
/*  NFA::complement                                                */
/*                                                                 */
/*******************************************************************/
FA &NFA::operator !( void )
{
    return !(toDFA());
}
/*******************************************************************/
/*                                                                 */
/*  NFA::getSubNfa                                                 */
/*                                                                 */
/*******************************************************************/
FA &NFA::getSubFA(State *iState, State *fState)
{
    NFA *nfa = new NFA();
    if(!initialState) return *nfa;
    nfa->setAlphabet(alphabet);
    NFAState* iniState = nfa->mkNFAInitialState();
    State2Map state2Map;
    StateSetIter iter;
    state2Map[iState] = iniState;
    nfa->makeCopyTransByNFA(dynamic_cast<NFAState*>(iState), state2Map);
    for(iter = nfa->finalStateSet.begin(); iter != nfa->finalStateSet.end(); iter++) (*iter)->setFinalFlag(0);
    nfa->finalStateSet.clear();
    NFAState* nfaState = dynamic_cast<NFAState*>(state2Map[fState]);
    nfaState->setFinalFlag(1);
    nfa->finalStateSet.insert(nfaState);
    nfa->removeDeadState();
    return *nfa;
}
/*******************************************************************/
/*                                                                 */
/*  NFA::removeUnreachableState                                    */
/*                                                                 */
/*******************************************************************/
void NFA::removeUnreachableState()
{
    
}
/*******************************************************************/
/*                                                                 */
/*  NFA::removeDeadState                                           */
/*                                                                 */
/*******************************************************************/
void NFA::removeDeadState()
{

}
/*******************************************************************/
/*                                                                 */
/*  NFA::getOneRun                                                 */
/*                                                                 */
/*******************************************************************/
Word NFA::getOneRun()
{
    Word w;
    return w;
}
/*******************************************************************/
/*                                                                 */
/*  NFA::isReachability                                            */
/*                                                                 */
/*******************************************************************/
bool NFA::isReachability(Word word)
{
    return true;
}

/*******************************************************************/
/*                                                                 */
/*  NFA::toDFA                                                     */
/*                                                                 */
/*******************************************************************/

bool FA::hasFinalState(const StateSet &stateSet)const
{
    StateSetConstIter iter;
    for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
        if((*iter)->isFinal()) return true;
    return false;
}

const NFATransMap NFA::getTransMapByStateSet(const StateSet& stateSet)const
{
    NFATransMap map;
    StateSetConstIter iter;
    NFATransMapIter mapIter;
    for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
    {
        NFATransMap transmap = dynamic_cast<NFAState*>(*iter)->getNFATransMap();
        for(mapIter = transmap.begin(); mapIter != transmap.end(); mapIter++)
            if(mapIter->first != EPSILON)
            {
                StateSet set = dynamic_cast<NFAState*>(*iter)->getTargetStateSetByChar(mapIter->first);
                map[mapIter->first].insert(set.begin(), set.end());
            }
    }
    return map;
}

void NFA::makeDFATrans(DFAState* preState, SetMapping &setMapping, const NFATransMap &nfaTransMap, DFA* dfa)const
{
    NFATransMapConstIter mapIter;
    for(mapIter = nfaTransMap.begin(); mapIter != nfaTransMap.end(); mapIter++)
    {
        SetMapping::iterator setMapIter = setMapping.find(mapIter->second);
        DFAState* postState;
        if(setMapIter == setMapping.end())
        {
            if(hasFinalState(mapIter->second)) postState = dfa->mkDFAFinalState();
            else postState = dfa->mkDFAState();
            setMapping[mapIter->second] = postState;
            makeDFATrans(postState, setMapping, getTransMapByStateSet(mapIter->second), dfa);
        }
        else postState = dynamic_cast<DFAState*>(setMapping[mapIter->second]);
        preState->addDFATrans(mapIter->first, postState);
//        cout<<preState->getID()<<" "<<mapIter->first<<" "<<postState->getID()<<endl;
//        cout<<postState->isFinal()<<endl;
    }
}

DFA &NFA::toDFA()
{
    DFA *dfa = new DFA();
    if(!initialState) return *dfa;
    dfa->setAlphabet(alphabet);
    StateSet set = dynamic_cast<NFAState*>(initialState)->getEpsilonClosure();
    set.insert(initialState);
    StateSetIter iter;
    SetMapping setMapping;
    NFATransMap nfaTransMap = getTransMapByStateSet(set);
    NFATransMapIter mapIter;
    DFAState *initialState = dfa->mkDFAInitialState();
    setMapping[set] = initialState;
    makeDFATrans(initialState, setMapping, nfaTransMap, dfa);
    dfa->setReachableFlag(1);
    if(dfa->finalStateSet.size() == 0) return dynamic_cast<DFA&>(FA::EmptyFA());
    return *dfa;
}
/*******************************************************************/
/*                                                                 */
/*  NFA::toNFA                                                     */
/*                                                                 */
/*******************************************************************/
NFA& NFA::toNFA()
{
    return *this;
}
/*******************************************************************/
/*                                                                 */
/*  NFA::postStar                                                  */
/*                                                                 */
/*******************************************************************/

void NFA::addPostStarNeedMap(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap &needMap, Need2Map& need2Map)
{
    if(addNeedMap(sState, sc, tState, tc, needMap, need2Map))
        addPostStarTrans(sState, sc, tState, tc, needMap, need2Map);
}

void NFA::addPostStarNeed2Map(NFAState* sState, Character sc, NFAState* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map)
{
    if(addNeed2Map(sState, sc, tState, tc1, tc2, needMap, need2Map))
    {
        NFAState* midState = mkNFAState();
        sState->addNFATrans(sc, midState);
        addPostStarTrans(sState, sc, midState, needMap, need2Map);
        addPostStarNeedMap(midState, tc2, tState, tc1, needMap, need2Map);
    }
}

void NFA::addPostStarTrans(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap& needMap, Need2Map& need2Map)
{
    StateSet stateset = tState->getTargetStateSetByChar(tc);
    for(StateSetIter iter = stateset.begin(); iter != stateset.end(); iter++)
        if(sState->addNFATrans(sc, dynamic_cast<NFAState*>(*iter)))
            addPostStarTrans(sState, sc, dynamic_cast<NFAState*>(*iter), needMap, need2Map);
}

void NFA::addPostStarTrans(NFAState *sState, Character c, NFAState *tState, NeedMap& needMap, Need2Map& need2Map)
{
    Char2StateCharSetMap map = needMap[sState];
    Char2StateCharSetMapIter mapIter = map.find(c);
    if(mapIter != map.end())
    {
        StateCharSet stateCharSet = mapIter->second;
        for(StateCharSetIter mIter = stateCharSet.begin(); mIter != stateCharSet.end(); mIter++)
            if(mIter->first->addNFATrans(mIter->second, tState))
                addPreStarTrans(mIter->first, mIter->second, tState, needMap, need2Map);
    }
}


NFA& NFA::postStar(const PDS& pds)//todo
{
    NFA* nfa = new NFA(*this);
    State2Map state2Map;
    NeedMap needMap;
    Need2Map need2Map;
    for(StateSetConstIter iter = pds.stateSet.begin(); iter != pds.stateSet.end(); iter++)
    {
        NFAState* state;
        if((*iter)->getID() != initialState->getID()) state = nfa->mkNFAState((*iter)->getID());
        else state = dynamic_cast<NFAState*>(nfa->initialState);
        state2Map[*iter] = state;
    }
    for(PDSTransListConstIter iter = pds.pdsPopTransList.begin(); iter != pds.pdsPopTransList.end(); iter++)
    {
        NFAState* sourceState = dynamic_cast<NFAState*>(state2Map[(*iter)->getSourceConfiguration()->getState()]);
        NFAState* targetState = dynamic_cast<NFAState*>(state2Map[(*iter)->getTargetConfiguration()->getState()]);
        Character character = dynamic_cast<ReplaceConfiguration*>((*iter)->getSourceConfiguration())->getStack();
        nfa->addPostStarNeedMap(targetState, EPSILON, sourceState, character, needMap, need2Map);
        
    }
    for(PDSTransListConstIter iter = pds.pdsReplaceTransList.begin(); iter != pds.pdsReplaceTransList.end();iter++)
    {
        NFAState* sourceState = dynamic_cast<NFAState*>(state2Map[(*iter)->getSourceConfiguration()->getState()]);
        NFAState* targetState = dynamic_cast<NFAState*>(state2Map[(*iter)->getTargetConfiguration()->getState()]);
        Character sourceChar = dynamic_cast<ReplaceConfiguration*>((*iter)->getSourceConfiguration())->getStack();
        Character targetChar = dynamic_cast<ReplaceConfiguration*>((*iter)->getTargetConfiguration())->getStack();
        nfa->addPostStarNeedMap(targetState, targetChar, sourceState, sourceChar, needMap, need2Map);
    }
    for(PDSTransListConstIter iter = pds.pdsPushTransList.begin(); iter != pds.pdsPushTransList.end();iter++)
    {
        NFAState* sourceState = dynamic_cast<NFAState*>(state2Map[(*iter)->getSourceConfiguration()->getState()]);
        NFAState* targetState = dynamic_cast<NFAState*>(state2Map[(*iter)->getTargetConfiguration()->getState()]);
        Character sourceChar = dynamic_cast<ReplaceConfiguration*>((*iter)->getSourceConfiguration())->getStack();
        Char2 char2 = dynamic_cast<PushConfiguration*>((*iter)->getTargetConfiguration())->getStack();
        nfa->addPostStarNeed2Map(targetState, char2.first, sourceState, sourceChar, char2.second, needMap, need2Map);
    }
    return *nfa;
}
/*******************************************************************/
/*                                                                 */
/*  NFA::preStar                                                   */
/*                                                                 */
/*******************************************************************/

bool NFA::addNeedMap(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap &needMap, Need2Map& need2Map)
{
    StateChar stateChar(sState, sc);
    StateCharSet stateCharSet;
    stateCharSet.insert(stateChar);
    NeedMapIter needmapIter = needMap.find(tState);
    if(needmapIter != needMap.end())
    {
        Char2StateCharSetMapIter mapIter = needmapIter->second.find(tc);
        if(mapIter != needmapIter->second.end())
        {
            if(!mapIter->second.insert(stateChar).second)
                return false;
        }
        else
            needmapIter->second[tc] = stateCharSet;
    }
    else
    {
        Char2StateCharSetMap char2StateCharSetMap;
        char2StateCharSetMap[tc] = stateCharSet;
        needMap[tState] = char2StateCharSetMap;
    }
    return true;
}
bool NFA::addNeed2Map(NFAState *sState, Character sc, NFAState *tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map &need2Map)
{
    StateChar stateChar(sState, sc);
    StateChar2 stateChar2(stateChar, tc2);
    StateChar2Set stateChar2Set;
    stateChar2Set.insert(stateChar2);
    Need2MapIter need2MapIter = need2Map.find(tState);
    if(need2MapIter != need2Map.end())
    {
        Char2StateChar2SetMapIter mapIter = need2MapIter->second.find(tc1);
        if(mapIter != need2MapIter->second.end())
        {
            if(!mapIter->second.insert(stateChar2).second)
                return false;
        }
        else
            need2MapIter->second[tc1] = stateChar2Set;
    }
    else
    {
        Char2StateChar2SetMap char2StateChar2SetMap;
        char2StateChar2SetMap[tc1] = stateChar2Set;
        need2Map[tState] = char2StateChar2SetMap;
    }
    return true;
}

void NFA::addPreStarNeedMap(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap &needMap, Need2Map& need2Map)
{
    if(addNeedMap(sState, sc, tState, tc, needMap, need2Map))
        addPreStarTrans(sState, sc, tState, tc, needMap, need2Map);
}

void NFA::addPreStarNeed2Map(NFAState* sState, Character sc, NFAState* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map)
{
    if(addNeed2Map(sState, sc, tState, tc1, tc2, needMap, need2Map))
    {
        StateSet stateset = tState->getTargetStateSetByChar(tc1);
        for(StateSetIter iter = stateset.begin(); iter != stateset.end(); iter++)
            addPreStarNeedMap(sState, sc, dynamic_cast<NFAState*>(*iter), tc2, needMap, need2Map);
    }
}

void NFA::addPreStarTrans(NFAState *sState, Character sc, NFAState *tState, Character tc, NeedMap& needMap, Need2Map& need2Map)
{
    StateSet stateset = tState->getTargetStateSetByChar(tc);
    for(StateSetIter iter = stateset.begin(); iter != stateset.end(); iter++)
        if(sState->addNFATrans(sc, dynamic_cast<NFAState*>(*iter)))
            addPreStarTrans(sState, sc, dynamic_cast<NFAState*>(*iter), needMap, need2Map);
}
void NFA::addPreStarTrans(NFAState *sState, Character c, NFAState *tState, NeedMap& needMap, Need2Map& need2Map)
{
    Char2StateCharSetMap map = needMap[sState];
    Char2StateCharSetMapIter mapIter = map.find(c);
    if(mapIter != map.end())
    {
        StateCharSet stateCharSet = mapIter->second;
        for(StateCharSetIter mIter = stateCharSet.begin(); mIter != stateCharSet.end(); mIter++)
            if(mIter->first->addNFATrans(mIter->second, tState))
                addPreStarTrans(mIter->first, mIter->second, tState, needMap, need2Map);
    }
    Char2StateChar2SetMap map2 = need2Map[sState];
    Char2StateChar2SetMapIter map2Iter = map2.find(c);
    if(map2Iter != map2.end())
    {
        StateChar2Set stateChar2Set = map2Iter->second;
        for(StateChar2SetIter mIter = stateChar2Set.begin(); mIter != stateChar2Set.end(); mIter++)
            addPreStarNeedMap(mIter->first.first, mIter->first.second, tState, mIter->second, needMap, need2Map);
    }
}
NFA& NFA::preStar(const PDS& pds)
{
    NFA* nfa = new NFA(*this);
    State2Map state2Map;
    NeedMap needMap;
    Need2Map need2Map;
    for(StateSetConstIter iter = pds.stateSet.begin(); iter != pds.stateSet.end(); iter++)
    {
        NFAState* state;
        if((*iter)->getID() != initialState->getID()) state = nfa->mkNFAState((*iter)->getID());
        else state = dynamic_cast<NFAState*>(nfa->initialState);
        state2Map[*iter] = state;
    }
    for(PDSTransListConstIter iter = pds.pdsPopTransList.begin(); iter != pds.pdsPopTransList.end(); iter++)
    {
        NFAState* sourceState = dynamic_cast<NFAState*>(state2Map[(*iter)->getSourceConfiguration()->getState()]);
        NFAState* targetState = dynamic_cast<NFAState*>(state2Map[(*iter)->getTargetConfiguration()->getState()]);
        Character character = dynamic_cast<ReplaceConfiguration*>((*iter)->getSourceConfiguration())->getStack();
        sourceState->addNFATrans(character, targetState);
    }
    for(PDSTransListConstIter iter = pds.pdsReplaceTransList.begin(); iter != pds.pdsReplaceTransList.end();iter++)
    {
        NFAState* sourceState = dynamic_cast<NFAState*>(state2Map[(*iter)->getSourceConfiguration()->getState()]);
        NFAState* targetState = dynamic_cast<NFAState*>(state2Map[(*iter)->getTargetConfiguration()->getState()]);
        Character sourceChar = dynamic_cast<ReplaceConfiguration*>((*iter)->getSourceConfiguration())->getStack();
        Character targetChar = dynamic_cast<ReplaceConfiguration*>((*iter)->getTargetConfiguration())->getStack();
        nfa->addPreStarNeedMap(sourceState, sourceChar, targetState, targetChar, needMap, need2Map);
    }
    for(PDSTransListConstIter iter = pds.pdsPushTransList.begin(); iter != pds.pdsPushTransList.end();iter++)
    {
        NFAState* sourceState = dynamic_cast<NFAState*>(state2Map[(*iter)->getSourceConfiguration()->getState()]);
        NFAState* targetState = dynamic_cast<NFAState*>(state2Map[(*iter)->getTargetConfiguration()->getState()]);
        Character sourceChar = dynamic_cast<ReplaceConfiguration*>((*iter)->getSourceConfiguration())->getStack();
        Char2 char2 = dynamic_cast<PushConfiguration*>((*iter)->getTargetConfiguration())->getStack();
        nfa->addPreStarNeed2Map(sourceState, sourceChar, targetState, char2.first, char2.second, needMap, need2Map);
    }
    return *nfa;

}
/*******************************************************************/
/*                                                                 */
/*  DFA::mkDFAState                                                */
/*                                                                 */
/*******************************************************************/
DFAState *DFA::mkDFAState()
{
    DFAState *dfaState = new DFAState();
    stateSet.insert(dfaState);
    return dfaState;
}
DFAState *DFA::mkDFAInitialState()
{
    DFAState *dfaState = new DFAState();
    initialState = dfaState;
    stateSet.insert(dfaState);
    return dfaState;
}
DFAState *DFA::mkDFAFinalState()
{
    DFAState *dfaState = new DFAState();
    dfaState->setFinalFlag(1);
    finalStateSet.insert(dfaState);
    stateSet.insert(dfaState);
    return dfaState;
}

/*******************************************************************/
/*                                                                 */
/*  DFA::copyConstruction                                          */
/*                                                                 */
/*******************************************************************/
void DFA::makeCopyTrans(DFAState *state, State2Map& state2map)
{
    DFATransMapIter mapIter;
    DFATransMap map = state->getDFATransMap();
    DFAState* preState = dynamic_cast<DFAState*>(state2map[state]);
    for(mapIter = map.begin(); mapIter != map.end(); mapIter++)
    {
        DFAState* postState;
        State2MapIter state2MapIter = state2map.find(mapIter->second);
        if(state2MapIter == state2map.end())
        {
            if(mapIter->second->isFinal()) postState = mkDFAFinalState();
            else postState = mkDFAState();
            state2map[mapIter->second] = postState;
            makeCopyTrans(dynamic_cast<DFAState*>(mapIter->second), state2map);
        }
        else postState = dynamic_cast<DFAState*>(state2MapIter->second);
        preState->addDFATrans(mapIter->first, postState);
    }
}
DFA::DFA(const DFA& dfa)
{
    if(dfa.initialState)
    {
        setAlphabet(dfa.alphabet);
        DFAState* iniState = mkDFAInitialState();
        State2Map state2Map;
        state2Map[dfa.initialState] = iniState;
        makeCopyTrans(dynamic_cast<DFAState*>(dfa.initialState), state2Map);
    }
}
/*******************************************************************/
/*                                                                 */
/*  DFA::ConstructionByParser                                      */
/*                                                                 */
/*******************************************************************/
DFA::DFA(RawFaData& data)
{
    flag = 0;
    RawFaDataWithInt* rawdata = dynamic_cast<RawFaDataWithInt*>(data.alphabetAndTransitions);
    setAlphabet(rawdata->alphabet);
    int stateNum = data.stateNumber;
    vector<DFAState*> stateVector;
    for(int pos = 0; pos < stateNum; pos++)
    {
        if(pos == data.initialState) stateVector.push_back(mkDFAInitialState());
        else if(data.finalStates.find(pos) != data.finalStates.end()) stateVector.push_back(mkDFAFinalState());
        else stateVector.push_back(mkDFAState());
    }
    if(data.finalStates.find(data.initialState) != data.finalStates.end())
    {
        initialState->setFinalFlag(1);
        finalStateSet.insert(initialState);
    }
    vector<tuple<int, int, int> > transVector = rawdata->transitions;
    for(int i = 0; i < transVector.size(); i++)
    {
        int sourceState = get<0>(transVector[i]);
        int targetState = get<2>(transVector[i]);
        int character = get<1>(transVector[i]);
        stateVector[sourceState]->addDFATrans(character, stateVector[targetState]);
    }
}
/*******************************************************************/
/*                                                                 */
/*  DFA::intersection                                              */
/*                                                                 */
/*******************************************************************/

const DFAIntersectionMap DFA::getTransMapByStatePair(const StatePair &statePair)
{
    DFAIntersectionMap dfaInterMap;
    DFAState* state1 = dynamic_cast<DFAState*>(statePair.first);
    DFAState* state2 = dynamic_cast<DFAState*>(statePair.second);
    if(state1->TransMapSize() > state2->TransMapSize())
    {
        state1 = dynamic_cast<DFAState*>(statePair.second);
        state2 = dynamic_cast<DFAState*>(statePair.first);
    }
    DFATransMap map1 = state1->getDFATransMap();
    DFATransMap map2 = state2->getDFATransMap();
    DFATransMapIter mapIter1;
    for(mapIter1 = map1.begin(); mapIter1 != map1.end(); mapIter1++)
    {
        DFATransMapIter mapIter2 = map2.find(mapIter1->first);
        if(mapIter2 != map2.end())
            dfaInterMap[mapIter1->first] = StatePair(mapIter1->second, mapIter2->second);
    }
    return dfaInterMap;
}

void DFA::makeDFAIntersectionTrans(DFAState *preState, PairMapping &pairMapping, const DFAIntersectionMap &dfaIntersectionMap, DFA *dfa)
{
    DFAIntersectionMapConstIter mapIter;
    for(mapIter = dfaIntersectionMap.begin(); mapIter!= dfaIntersectionMap.end(); mapIter++)
    {
        PairMapping::iterator pairMapIter = pairMapping.find(mapIter->second);
        DFAState* postState;
        if(pairMapIter == pairMapping.end())
        {
            if(mapIter->second.first->isFinal() && mapIter->second.second->isFinal())
                postState = dfa->mkDFAFinalState();
            else postState = dfa->mkDFAState();
            pairMapping[mapIter->second] = postState;
            makeDFAIntersectionTrans(postState, pairMapping, getTransMapByStatePair(mapIter->second), dfa);
        }
        else postState = dynamic_cast<DFAState*>(pairMapping[mapIter->second]);
        preState->addDFATrans(mapIter->first, postState);
    }
}

FA &DFA::operator &(const FA &fa)
{
    DFA* dfa = new DFA();
    if(!initialState || !fa.initialState) return *dfa;
    Alphabet charSet(alphabet);
    charSet.insert(fa.alphabet.begin(), fa.alphabet.end());
    dfa->setAlphabet(charSet);
    StatePair statePair(initialState, fa.initialState);
    DFAState* iniState = dfa->mkDFAInitialState();
    PairMapping pairMapping;
    pairMapping[statePair] = iniState;
    makeDFAIntersectionTrans(iniState, pairMapping, getTransMapByStatePair(statePair), dfa);
    if(dfa->finalStateSet.size() == 0) return FA::EmptyFA();  //todo to make a NULLFA constFA
    dfa->setReachableFlag(1);
    return *dfa;
}

/*******************************************************************/
/*                                                                 */
/*  DFA::union                                                     */
/*                                                                 */
/*******************************************************************/
FA &DFA::operator |(const FA &fa)
{
    if(!initialState) return const_cast<FA&>(fa).toDFA();
    if(!fa.initialState) return toDFA();
    NFA nfa(*this);
    nfa.setAlphabet(alphabet);
    nfa.addAlphabet(fa.alphabet);
    NFA tempnfa = const_cast<FA&>(fa).toNFA();
    NFAState* tempNFAStete1 = dynamic_cast<NFAState*>(nfa.initialState);
    NFAState* tempNFAStete2 = dynamic_cast<NFAState*>(tempnfa.initialState);
    State2Map state2Map;
    NFAState* state = nfa.mkNFAState();
    state2Map[tempNFAStete2] = state;
    NFAState* iniState = nfa.mkNFAInitialState();
    nfa.makeCopyTransByNFA(tempNFAStete2, state2Map);
    iniState->addEpsilonTrans(tempNFAStete1);
    iniState->addEpsilonTrans(state);
    return nfa.toDFA();
}
/*******************************************************************/
/*                                                                 */
/*  DFA::concatination                                             */
/*                                                                 */
/*******************************************************************/
FA &DFA::operator +(const FA &fa)//todo
{
    if(!initialState) return const_cast<FA&>(fa).toDFA();
    if(!fa.initialState) return toDFA();
    NFA nfa(*this);
    nfa.addAlphabet(fa.alphabet);
    StateSetIter iter;
    StateSet fStateSet;
    fStateSet.insert(nfa.finalStateSet.begin(), nfa.finalStateSet.end());
    nfa.finalStateSet.clear();
    NFAState* state = nfa.mkNFAState();
    State2Map state2Map;
    NFA tempnfa = const_cast<FA&>(fa).toNFA();
    state2Map[tempnfa.initialState] = state;
    nfa.makeCopyTransByNFA(dynamic_cast<NFAState*>(tempnfa.initialState), state2Map);
    for(iter = fStateSet.begin(); iter != fStateSet.end(); iter++)
    {
        (*iter)->setFinalFlag(0);
        dynamic_cast<NFAState*>(*iter)->addEpsilonTrans(state);
    }
    return nfa.toDFA();
}
/*******************************************************************/
/*                                                                 */
/*  DFA::complement                                                */
/*                                                                 */
/*******************************************************************/

void DFA::makeDFAComplementTrans(DFAState *state, DFAState* trapState, State2Map &state2map, DFA *dfa)
{
    Alphabet charSet;
    AlphabetIter AIter;
    DFATransMap map = state->getDFATransMap();
    DFATransMapIter mapIter;
    DFAState* preState = dynamic_cast<DFAState*>(state2map[state]);
    for(mapIter = map.begin(); mapIter != map.end(); mapIter++)
    {
        DFAState* postState;
        State2MapIter state2MapIter = state2map.find(mapIter->second);
        if(state2MapIter == state2map.end())
        {
            if(mapIter->second->isFinal()) postState = dfa->mkDFAState();
            else postState = dfa->mkDFAFinalState();
            state2map[mapIter->second] = postState;
            makeDFAComplementTrans(dynamic_cast<DFAState*>(mapIter->second), trapState, state2map, dfa);
        }
        else postState = dynamic_cast<DFAState*>(state2MapIter->second);
        preState->addDFATrans(mapIter->first, postState);
        charSet.insert(mapIter->first);
    }
    for(AIter = alphabet.begin(); AIter != alphabet.end(); AIter++)
        if(charSet.find(*AIter) == charSet.end())
            preState->addDFATrans(*AIter, trapState);
}
FA &DFA::operator !( void )
{
    if(!initialState) return FA::CompleteFA(alphabet);
    DFA *dfa = new DFA();
    dfa->setAlphabet(alphabet);
    StateSetIter stateSetIter;
    State2Map state2Map;
    DFAState* iniState = dfa->mkDFAInitialState();
    DFAState* trapState = dfa->mkDFAFinalState();
    AlphabetIter AIter;
    for(AIter = alphabet.begin(); AIter != alphabet.end(); AIter++)
        trapState->addDFATrans(*AIter, trapState);
    if(!initialState->isFinal()) iniState->setFinalFlag(1);
    state2Map[initialState] = iniState;
    makeDFAComplementTrans(dynamic_cast<DFAState*>(initialState), trapState, state2Map, dfa);
    return *dfa;
}
/*******************************************************************/
/*                                                                 */
/*  DFA::minus                                                     */
/*                                                                 */
/*******************************************************************/
FA &DFA::operator -(const FA &fa)
{
    return *this & !(const_cast<FA&>(fa));
}

/*******************************************************************/
/*                                                                 */
/*  DFA::getSubNfa                                                 */
/*                                                                 */
/*******************************************************************/
FA &DFA::getSubFA(State *iState, State *fState)
{
    DFA *dfa = new DFA();
    if(!initialState) return *dfa;
    dfa->setAlphabet(alphabet);
    DFAState* state = dfa->mkDFAInitialState();
    State2Map state2Map;
    StateSetIter iter;
    state2Map[iState] = state;
    dfa->makeCopyTrans(dynamic_cast<DFAState*>(iState), state2Map);
    for(iter = dfa->finalStateSet.begin(); iter != dfa->finalStateSet.end(); iter++) (*iter)->setFinalFlag(0);
    dfa->finalStateSet.clear();
    DFAState* dfaState = dynamic_cast<DFAState*>(state2Map[fState]);
    dfaState->setFinalFlag(1);
    dfa->finalStateSet.insert(dfaState);
    dfa->removeDeadState();
    return *dfa;
}
/*******************************************************************/
/*                                                                 */
/*  DFA::removeUnreachableState                                    */
/*                                                                 */
/*******************************************************************/
const StateSet DFA::getReachableStateSet(const StateSet stateSet)
{
    StateSet reachableStateSet;
    for(StateSetConstIter iter = stateSet.begin(); iter != stateSet.end(); iter++)
    {
        StateSet tempSet = (*iter)->getTargetStateSet();
        reachableStateSet.insert(tempSet.begin(), tempSet.end());
    }
    return reachableStateSet;
}
const StateSet DFA::getReachableStateSet()
{
    StateSet stateSet;
    StateSet tempSet = initialState->getTargetStateSet();
    size_t iniSize = stateSet.size();
    size_t curSize = 1;
    while(iniSize != curSize)
    {
        iniSize = stateSet.size();
        stateSet.insert(tempSet.begin(), tempSet.end());
        tempSet = getReachableStateSet(tempSet);
        curSize = stateSet.size();
    }
    stateSet.insert(initialState);
    return stateSet;
}
void DFA::removeUnreachableState()
{
    if(!initialState) return;
    StateSet reachableStateSet = getReachableStateSet();
    if(!hasFinalState(reachableStateSet))
    {
        initialState = NULL;
        return;
    }
    if(reachableStateSet.size() != this->stateSet.size())
    {
        StateSetIter iter;
        StateSet set;
        for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
            if(reachableStateSet.find(*iter) == reachableStateSet.end())
            {
                StateSet postStateSet = (*iter)->getTargetStateSet();
                StateSetIter sIter;
                for(sIter = postStateSet.begin(); sIter != postStateSet.end(); sIter++)
                    if(reachableStateSet.find(*sIter) != reachableStateSet.end())
                        dynamic_cast<DFAState*>(*iter)->delDFATrans(*sIter);
                set.insert(*iter);
            }
        for(iter = set.begin(); iter != set.end(); iter++)
            stateSet.erase(*iter);
    }
}
/*******************************************************************/
/*                                                                 */
/*  DFA::removeDeadState                                           */
/*                                                                 */
/*******************************************************************/
const State2StateSetMap DFA::getReverseMap()
{
    State2StateSetMap state2stateSetMap;
    StateSetIter iter;
    for(iter = stateSet.begin(); iter != stateSet.end(); iter++) state2stateSetMap[*iter] = StateSet();
    for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
    {
        DFATransMap dfaTransMap = dynamic_cast<DFAState*>(*iter)->getDFATransMap();
        DFATransMapIter mapIter;
        for(mapIter = dfaTransMap.begin(); mapIter != dfaTransMap.end(); mapIter++)
            state2stateSetMap[mapIter->second].insert(*iter);
    }
    return state2stateSetMap;
}
const StateSet DFA::getRStateSetByStateSetAndRMap(const State2StateSetMap reverseMap, const StateSet stateSet)
{
    StateSet set;
    StateSetConstIter sIter;
    for(sIter = stateSet.begin(); sIter != stateSet.end(); sIter++)
    {
        State2StateSetMapConstIter mapIter = reverseMap.find(*sIter);
        if(mapIter != reverseMap.end()) set.insert(mapIter->second.begin(), mapIter->second.end());
    }
    return set;
}
const StateSet DFA::getLiveStateSet(const State2StateSetMap reverseMap, const StateSet stateSet)
{
    StateSet reverseSet;
    size_t iniSize = reverseSet.size();
    size_t curSize = 1;
    StateSet tempSet = getRStateSetByStateSetAndRMap(reverseMap, stateSet);
    while(iniSize != curSize)
    {
        iniSize = reverseSet.size();
        reverseSet.insert(tempSet.begin(), tempSet.end());
        tempSet = getRStateSetByStateSetAndRMap(reverseMap, tempSet);
        curSize = reverseSet.size();
    }
    reverseSet.insert(stateSet.begin(), stateSet.end());
    return reverseSet;
}
void DFA::removeDeadState()
{
    if(!initialState) return;
    StateSetIter iter;
    State2StateSetMap reverseMap = getReverseMap();
    StateSet liveStateSet = getLiveStateSet(reverseMap, finalStateSet);
    if(liveStateSet.find(initialState) == liveStateSet.end())
    {
        initialState = NULL;
        return;
    }
    StateSet set;
    for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
        if(liveStateSet.find(*iter) == liveStateSet.end())
        {
            StateSet preStateSet = reverseMap.find(*iter)->second;
            StateSetIter sIter;
            for(sIter = preStateSet.begin(); sIter != preStateSet.end(); sIter++)
                if(liveStateSet.find(*sIter) != liveStateSet.end())
                    dynamic_cast<DFAState*>(*sIter)->delDFATrans(*iter);
            set.insert(*iter);
        }
    for(iter = set.begin(); iter != set.end(); iter++)
        stateSet.erase(*iter);
}

/*******************************************************************/
/*                                                                 */
/*  DFA::getOneRun                                                 */
/*                                                                 */
/*******************************************************************/
Word DFA::getOneRun()
{
    Word w;
    
    return w;
}
/*******************************************************************/
/*                                                                 */
/*  DFA::isReachability                                            */
/*                                                                 */
/*******************************************************************/
bool DFA::isReachability(Word word)
{
    if(!initialState)
        return false;
    DFAState* state = dynamic_cast<DFAState*>(initialState);
    for(int i = 0; i < word.size(); i++)
    {
        state = state->getTargetStateByChar(word[i]) ;
        if(state->isNULL()) return false;
    }
    if(state->isFinal()) return true;
    return false;
}
/*******************************************************************/
/*                                                                 */
/*  DFA::minimize                                                  */
/*                                                                 */
/*******************************************************************/
DFA& DFA::minimize()
{
    DFA* dfa = new DFA();
    return *dfa;
}
/*******************************************************************/
/*                                                                 */
/*  DFA::toDFA                                                     */
/*                                                                 */
/*******************************************************************/
DFA& DFA::toDFA()
{
    return *this;
}
/*******************************************************************/
/*                                                                 */
/*  DFA::toNFA                                                     */
/*                                                                 */
/*******************************************************************/
NFA& DFA::toNFA()
{
    NFA* nfa = new NFA(*this);
    return *nfa;
}

