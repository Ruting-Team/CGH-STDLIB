/*****************************************************************************/
/*  FA.cpp                                                                   */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/4/8.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#include "FA.hpp"
using namespace cgh;
long State::counter = 0;
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
    if(epsilonClosure.size() == 0 && mapIter == nfaTransMap.end()) return stateSet;
    if(mapIter != nfaTransMap.end()) stateSet.insert(mapIter->second.begin(), mapIter->second.end());
    if(epsilonClosure.size() == 0 && mapIter != nfaTransMap.end())
        for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
        {
            StateSet epsilonSet = dynamic_cast<NFAState*>(*iter)->getEpsilonClosure();
            tempSet.insert(epsilonSet.begin(), epsilonSet.end());
        }
    else
        for(iter = epsilonClosure.begin(); iter != epsilonClosure.end(); iter++)
        {
            StateSet epsilonSet = dynamic_cast<NFAState*>(*iter)->getEpsilonClosure();
            tempSet.insert(epsilonSet.begin(), epsilonSet.end());
        }
    if(tempSet.size()>0) stateSet.insert(tempSet.begin(), tempSet.end());
    return stateSet;
}
/*******************************************************************/
/*                                                                 */
/*  NfaState::NFAMapIterator                                       */
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
/*  DFAState::addDFATrans                                          */
/*                                                                 */
/*******************************************************************/
bool DFAState::addDFATrans(Character character, DFAState *target)
{
    dfaTransMap[character] = target;
    return 1;
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
/*  FA::isEmpty                                                   */
/*                                                                 */
/*******************************************************************/
bool FA::isEmpty()
{
    if(!initialState) return true;
    if(!isReachable()) removeUnreachableState();
    if(finalStateSet.size() == 0) return true;
    return false;
}
FA& FA::EmptyFA()
{
    DFA* dfa = new DFA();
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
                makeCopyTransByDFA(dynamic_cast<DFAState*>(*iter), state2map);
            }
            else postState = dynamic_cast<NFAState*>(state2MapIter->second);
            preState->addNFATrans(mapIter->first, postState);
        }
    }
}
NFA::NFA(const NFA& nfa)
{
    setAlphabet(nfa.alphabet);
    NFAState* iniState = mkNFAInitialState();
    State2Map state2Map;
    state2Map[nfa.initialState] = iniState;
    makeCopyTransByNFA(dynamic_cast<NFAState*>(nfa.initialState), state2Map);
}
NFA::NFA(DFA& dfa)
{
    setAlphabet(dfa.alphabet);
    NFAState* iniState = mkNFAInitialState();
    State2Map state2Map;
    state2Map[dfa.initialState] = iniState;
    makeCopyTransByDFA(dynamic_cast<DFAState*>(dfa.initialState), state2Map);
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
NFAState *NFA::mkNFAInitialState()
{
    NFAState *nfaState = new NFAState();
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
    return new DFAState();
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
    NFA *nfa = new NFA();
    nfa->setAlphabet(alphabet);
    nfa->addAlphabet(fa.alphabet);
    NFAState* iniState = nfa->mkNFAInitialState();
    NFAState* state1 = nfa->mkNFAState();
    NFAState* state2 = nfa->mkNFAState();
    State2Map state2Map1;
    State2Map state2Map2;
    state2Map1[initialState] = state1;
    state2Map2[fa.initialState] = state2;
    nfa->makeCopyTransByNFA(dynamic_cast<NFAState*>(initialState), state2Map1);
    nfa->makeCopyTransByNFA(dynamic_cast<NFAState*>(fa.initialState), state2Map2);
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
    NFA *nfa = new NFA(*this);
    nfa->addAlphabet(fa.alphabet);
    StateSetIter iter;
    StateSet fStateSet;
    fStateSet.insert(nfa->finalStateSet.begin(), nfa->finalStateSet.end());
    nfa->finalStateSet.clear();
    NFAState* state = nfa->mkNFAState();
    State2Map state2Map;
    state2Map[fa.initialState] = state;
    nfa->makeCopyTransByNFA(dynamic_cast<NFAState*>(fa.initialState), state2Map);
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
    return const_cast<DFA&>(toDFA()) & !(const_cast<FA&>(fa));
}
/*******************************************************************/
/*                                                                 */
/*  NFA::complement                                                */
/*                                                                 */
/*******************************************************************/
FA &NFA::operator !( void )
{
    return !(const_cast<DFA&>(toDFA()));
}
/*******************************************************************/
/*                                                                 */
/*  NFA::getSubNfa                                                 */
/*                                                                 */
/*******************************************************************/
FA &NFA::getSubFA(State *iState, State *fState)
{
    NFA *nfa = new NFA();
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
    StateSetIter iter;
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
    setAlphabet(dfa.alphabet);
    DFAState* iniState = mkDFAInitialState();
    State2Map state2Map;
    state2Map[dfa.initialState] = iniState;
    makeCopyTrans(dynamic_cast<DFAState*>(dfa.initialState), state2Map);
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
    dfa->setAlphabet(alphabet);
    dfa->addAlphabet(fa.alphabet);
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
    NFA nfa(*this);
    nfa.setAlphabet(alphabet);
    nfa.addAlphabet(fa.alphabet);
    NFAState* tempNFAStete = dynamic_cast<NFAState*>(nfa.initialState);
    DFAState* tempDFAStete = dynamic_cast<DFAState*>(fa.initialState);
    State2Map state2Map;
    NFAState* state = nfa.mkNFAState();
    state2Map[tempDFAStete] = state;
    NFAState* iniState = nfa.mkNFAInitialState();
    nfa.makeCopyTransByDFA(tempDFAStete, state2Map);
    iniState->addEpsilonTrans(tempNFAStete);
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
    NFA nfa(*this);
    nfa.addAlphabet(fa.alphabet);
    StateSetIter iter;
    StateSet fStateSet;
    fStateSet.insert(nfa.finalStateSet.begin(), nfa.finalStateSet.end());
    nfa.finalStateSet.clear();
    NFAState* state = nfa.mkNFAState();
    State2Map state2Map;
    state2Map[fa.initialState] = state;
    nfa.makeCopyTransByDFA(dynamic_cast<DFAState*>(fa.initialState), state2Map);
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
    StateSet reachableStateSet = getReachableStateSet();
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
    StateSetIter iter;
    State2StateSetMap reverseMap = getReverseMap();
    StateSet liveStateSet = getLiveStateSet(reverseMap, finalStateSet);
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
const StateSet DFA::getStateSetByStateSetAndChar(const StateSet &stateSet, Character character)
{
    StateSet set;
    StateSetConstIter sIter;
    for(sIter = stateSet.begin(); sIter != stateSet.end(); sIter++)
        if(DFAState* state = dynamic_cast<DFAState*>((*sIter))->getTargetStateByChar(character)) set.insert(state);
    return set;
}
bool DFA::isReachability(Word word)
{
    StateSet set;
    set.insert(initialState);
    for(int i = 0; i < word.size(); i++)
    {
        set = getStateSetByStateSetAndChar(set, word[i]);
        if(set.size() == 0) return false;
    }
    if(hasFinalState(set)) return true;
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

