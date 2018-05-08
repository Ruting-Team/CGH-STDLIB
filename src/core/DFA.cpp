//
//  DFA.cpp
//  CGH
//
//  Created by 何锦龙 on 2018/5/7.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#include "FA.hpp"
using namespace cgh;
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
    DFATransMap& map = state->getDFATransMap();
    DFAState* preState = dynamic_cast<DFAState*>(state2map[state]);
    for(DFATransMapIter mapIter = map.begin(); mapIter != map.end(); mapIter++)
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

void DFA::getTransMapByStatePair(const StatePair &statePair, DFAIntersectionMap& dfaIntersectionMap)
{
    DFAState* state1 = dynamic_cast<DFAState*>(statePair.first);
    DFAState* state2 = dynamic_cast<DFAState*>(statePair.second);
    if(state1->TransMapSize() > state2->TransMapSize())
    {
        state1 = dynamic_cast<DFAState*>(statePair.second);
        state2 = dynamic_cast<DFAState*>(statePair.first);
    }
    DFATransMap &map1 = state1->getDFATransMap();
    DFATransMap &map2 = state2->getDFATransMap();
    for(DFATransMapIter mapIter1 = map1.begin(); mapIter1 != map1.end(); mapIter1++)
    {
        DFATransMapIter mapIter2 = map2.find(mapIter1->first);
        if(mapIter2 != map2.end())
            dfaIntersectionMap[mapIter1->first] = StatePair(mapIter1->second, mapIter2->second);
    }
}

void DFA::makeDFAIntersectionTrans(DFAState *preState, PairMapping &pairMapping, const DFAIntersectionMap &dfaIntersectionMap, DFA *dfa)
{
    DFAIntersectionMap map;
    for(DFAIntersectionMapConstIter mapIter = dfaIntersectionMap.begin(); mapIter!= dfaIntersectionMap.end(); mapIter++)
    {
        PairMapping::iterator pairMapIter = pairMapping.find(mapIter->second);
        DFAState* postState;
        if(pairMapIter == pairMapping.end())
        {
            map.clear();
            getTransMapByStatePair(mapIter->second, map);
            if(mapIter->second.first->isFinal() && mapIter->second.second->isFinal())
                postState = dfa->mkDFAFinalState();
            else postState = dfa->mkDFAState();
            pairMapping[mapIter->second] = postState;
            makeDFAIntersectionTrans(postState, pairMapping, map, dfa);
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
    DFAIntersectionMap dfaIntersectionMap;
    getTransMapByStatePair(statePair, dfaIntersectionMap);
    makeDFAIntersectionTrans(iniState, pairMapping, dfaIntersectionMap, dfa);
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
    if(!initialState) return const_cast<FA&>(fa).determine();
    if(!fa.initialState) return determine();
    NFA nfa(*this);
    nfa.setAlphabet(alphabet);
    nfa.addAlphabet(fa.alphabet);
    NFA tempnfa = const_cast<FA&>(fa).nondetermine();
    NFAState* tempNFAStete1 = dynamic_cast<NFAState*>(nfa.initialState);
    NFAState* tempNFAStete2 = dynamic_cast<NFAState*>(tempnfa.initialState);
    State2Map state2Map;
    NFAState* state = nfa.mkNFAState();
    state2Map[tempNFAStete2] = state;
    NFAState* iniState = nfa.mkNFAInitialState();
    nfa.makeCopyTransByNFA(tempNFAStete2, state2Map);
    iniState->addEpsilonTrans(tempNFAStete1);
    iniState->addEpsilonTrans(state);
    return nfa.determine();
}
/*******************************************************************/
/*                                                                 */
/*  DFA::concatination                                             */
/*                                                                 */
/*******************************************************************/
FA &DFA::concat(const FA &fa)//todo
{
    if(!initialState) return const_cast<FA&>(fa).determine();
    if(!fa.initialState) return determine();
    NFA nfa(*this);
    nfa.addAlphabet(fa.alphabet);
    StateSetIter iter;
    StateSet fStateSet;
    fStateSet.insert(nfa.finalStateSet.begin(), nfa.finalStateSet.end());
    nfa.finalStateSet.clear();
    NFAState* state = nfa.mkNFAState();
    State2Map state2Map;
    NFA tempnfa = const_cast<FA&>(fa).nondetermine();
    state2Map[tempnfa.initialState] = state;
    nfa.makeCopyTransByNFA(dynamic_cast<NFAState*>(tempnfa.initialState), state2Map);
    for(iter = fStateSet.begin(); iter != fStateSet.end(); iter++)
    {
        (*iter)->setFinalFlag(0);
        dynamic_cast<NFAState*>(*iter)->addEpsilonTrans(state);
    }
    return nfa.determine();
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
FA &DFA::minus(const FA &fa)
{
    return *this & !(const_cast<FA&>(fa));
}

/*******************************************************************/
/*                                                                 */
/*  DFA::subset                                                    */
/*                                                                 */
/*******************************************************************/
FA &DFA::subset(State *iState, State *fState)
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
void DFA::getReachableStateSet(StateSet& reachableStateSet, StateSet& workSet)
{
    if(workSet.size() == 0) return;
    StateSet tempSet;
    StateSet set;
    for(StateSetIter iter = workSet.begin(); iter != workSet.end(); iter++)
    {
        tempSet.clear();
        dynamic_cast<DFAState*>(*iter)->getTargetStateSet(tempSet);
        for(StateSetIter iter = tempSet.begin(); iter != tempSet.end(); iter++)
            if(reachableStateSet.insert(*iter).second)
                set.insert(*iter);
    }
    getReachableStateSet(reachableStateSet, set);
}
void DFA::removeUnreachableState()
{
    if(!initialState) return;
    StateSet reachableStateSet;
    StateSet workSet;
    workSet.insert(initialState);
    reachableStateSet.insert(initialState);
    getReachableStateSet(reachableStateSet, workSet);
    if(!hasFinalState(reachableStateSet))
    {
        initialState = NULL;
        return;
    }
    if(reachableStateSet.size() != this->stateSet.size())
    {
        StateSet set;
        for(StateSetIter iter = stateSet.begin(); iter != stateSet.end(); iter++)
            if(reachableStateSet.find(*iter) == reachableStateSet.end())
            {
                StateSet postStateSet = (*iter)->getTargetStateSet();
                StateSetIter sIter;
                for(sIter = postStateSet.begin(); sIter != postStateSet.end(); sIter++)
                    if(reachableStateSet.find(*sIter) != reachableStateSet.end())
                        dynamic_cast<DFAState*>(*iter)->delDFATrans(*sIter);
                set.insert(*iter);
            }
        for(StateSetIter iter = set.begin(); iter != set.end(); iter++)
        {
            delete *iter;
            stateSet.erase(*iter);
        }
    }
    setReachableFlag(1);
}
/*******************************************************************/
/*                                                                 */
/*  DFA::removeDeadState                                           */
/*                                                                 */
/*******************************************************************/
void DFA::getReverseMap(State2StateSetMap& reverseMap)
{
    for(StateSetIter iter = stateSet.begin(); iter != stateSet.end(); iter++)
        reverseMap[*iter] = StateSet();
    for(StateSetIter iter = stateSet.begin(); iter != stateSet.end(); iter++)
    {
        DFATransMap &dfaTransMap = dynamic_cast<DFAState*>(*iter)->getDFATransMap();
        for(DFATransMapIter mapIter = dfaTransMap.begin(); mapIter != dfaTransMap.end(); mapIter++)
            reverseMap[mapIter->second].insert(*iter);
    }
}

void DFA::getLiveStateSet(const State2StateSetMap& reverseMap, StateSet& liveStateSet, StateSet& workSet)
{
    if(workSet.size() == 0) return;
    StateSet set;
    State2StateSetMapConstIter mapConstIter;
    for(StateSetIter iter = workSet.begin(); iter != workSet.end(); iter++)
    {
        mapConstIter = reverseMap.find(*iter);
        for(StateSetIter iter = mapConstIter->second.begin(); iter != mapConstIter->second.end(); iter++)
            if(liveStateSet.insert(*iter).second)
                set.insert(*iter);
    }
    getLiveStateSet(reverseMap, liveStateSet, set);
}
void DFA::removeDeadState()
{
    if(!initialState) return;
    State2StateSetMap reverseMap;
    getReverseMap(reverseMap);
    StateSet liveStateSet(finalStateSet.begin(), finalStateSet.end());
    getLiveStateSet(reverseMap, liveStateSet, finalStateSet);
    if(liveStateSet.find(initialState) == liveStateSet.end())
    {
        initialState = NULL;
        return;
    }
    StateSet set;
    for(StateSetIter iter = stateSet.begin(); iter != stateSet.end(); iter++)
        if(liveStateSet.find(*iter) == liveStateSet.end())
        {
            StateSet preStateSet = reverseMap.find(*iter)->second;
            StateSetIter sIter;
            for(sIter = preStateSet.begin(); sIter != preStateSet.end(); sIter++)
                if(liveStateSet.find(*sIter) != liveStateSet.end())
                    dynamic_cast<DFAState*>(*sIter)->delDFATrans(*iter);
            set.insert(*iter);
        }
    for(StateSetIter iter = set.begin(); iter != set.end(); iter++)
    {
        delete *iter;
        stateSet.erase(*iter);
    }
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
bool DFA::isReachable(Word word)
{
    if(!initialState)
        return false;
    DFAState* state = dynamic_cast<DFAState*>(initialState);
    for(int i = 0; i < word.size(); i++)
    {
        state = state->getTargetStateByChar(word[i]) ;
        if(!state) return false;
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
DFA& DFA::determine()
{
    return *this;
}
/*******************************************************************/
/*                                                                 */
/*  DFA::toNFA                                                     */
/*                                                                 */
/*******************************************************************/
NFA& DFA::nondetermine()
{
    NFA* nfa = new NFA(*this);
    return *nfa;
}
