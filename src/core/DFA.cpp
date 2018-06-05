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
        if(dfa.initialState->isFinal())
            addFinalState(iniState);
        State2Map state2Map;
        state2Map[dfa.initialState] = iniState;
        makeCopyTrans(dynamic_cast<DFAState*>(dfa.initialState), state2Map);
        setDeterminateFlag(1);
    }
}
/*******************************************************************/
/*                                                                 */
/*  DFA::ConstructionByParser                                      */
/*                                                                 */
/*******************************************************************/
DFA::DFA(const RawFaData& data)
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
    setDeterminateFlag(1);
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
    dfa->setAlphabet(alphabet);
    DFA *tempDFA = NULL;
    if(!fa.isDeterminate()) tempDFA = &dynamic_cast<const NFA&>(fa).determine();
    else tempDFA = &dynamic_cast<DFA&>(const_cast<FA&>(fa));
    StatePair statePair(initialState, tempDFA->initialState);
    DFAState* iniState = dfa->mkDFAInitialState();
    if(initialState->isFinal() && tempDFA->initialState->isFinal()) dfa->addFinalState(iniState);
    PairMapping pairMapping;
    pairMapping[statePair] = iniState;
    DFAIntersectionMap dfaIntersectionMap;
    getTransMapByStatePair(statePair, dfaIntersectionMap);
    makeDFAIntersectionTrans(iniState, pairMapping, dfaIntersectionMap, dfa);
    if(!fa.isDeterminate()) delete tempDFA;
    if(dfa->finalStateSet.size() == 0)
    {
        delete dfa;
        return FA::EmptyFA();
    }
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
    if(!initialState) return const_cast<FA&>(fa);
    if(!fa.initialState) return *this;
    NFA nfa(*this);
    return (nfa | fa);
}
/*******************************************************************/
/*                                                                 */
/*  DFA::concatination                                             */
/*                                                                 */
/*******************************************************************/
FA &DFA::concat(const FA &fa)
{
    if(!initialState) return const_cast<FA&>(fa);
    if(!fa.initialState) return *this;
    NFA nfa(*this);
    return nfa.concat(fa);
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
    DFATransMap& map = state->getDFATransMap();
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
    for(AlphabetIter AIter = alphabet.begin(); AIter != alphabet.end(); AIter++)
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
    DFA* cDFA = dynamic_cast<DFA*>(&(!(const_cast<FA&>(fa))));
    DFA* iDFA = dynamic_cast<DFA*>(&(*this & (*cDFA)));
    delete cDFA;
    return *iDFA;
}

/*******************************************************************/
/*                                                                 */
/*  DFA::subset                                                    */
/*                                                                 */
/*******************************************************************/
FA &DFA::subset(const State *iState, const State *fState)
{
    DFA *dfa = new DFA();
    if(!initialState) return *dfa;
    dfa->setAlphabet(alphabet);
    DFAState* state = dfa->mkDFAInitialState();
    State2Map state2Map;
    StateSetIter iter;
    state2Map[const_cast<State*>(iState)] = state;
    dfa->makeCopyTrans(dynamic_cast<DFAState*>(const_cast<State*>(iState)), state2Map);
    dfa->clearFinalStateSet();
    DFAState* dfaState = dynamic_cast<DFAState*>(state2Map[const_cast<State*>(fState)]);
    dfa->addFinalState(dfaState);
    dfa->removeDeadState();
    return *dfa;
}

/*******************************************************************/
/*                                                                 */
/*  DFA::rightQuotient                                             */
/*                                                                 */
/*******************************************************************/

FA& DFA::rightQuotient(Character character)
{
    DFA* dfa = new DFA(*this);
    StateSet finSteteSet;
    for(StateSetIter iter = dfa->stateSet.begin(); iter != dfa->stateSet.end(); iter++)
    {
        State* state = dynamic_cast<DFAState*>(*iter)->getTargetStateByChar(character);
        if(state && state->isFinal())
            finSteteSet.insert(*iter);
    }
    dfa->clearFinalStateSet();
    for(StateSetIter iter = finSteteSet.begin(); iter != finSteteSet.end(); iter++)
        dfa->addFinalState(*iter);
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
    if(!FA::hasFinalState(reachableStateSet))
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
                for(StateSetIter sIter = postStateSet.begin(); sIter != postStateSet.end(); sIter++)
                    if(reachableStateSet.find(*sIter) != reachableStateSet.end())
                        dynamic_cast<DFAState*>(*iter)->delDFATrans(*sIter);
                set.insert(*iter);
            }
        for(StateSetIter iter = set.begin(); iter != set.end(); iter++)
        {
            stateSet.erase(*iter);
            delete *iter;
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
    for(StateSetIter iter = workSet.begin(); iter != workSet.end(); iter++)
    {
        State2StateSetMapConstIter mapConstIter = reverseMap.find(*iter);
        if(mapConstIter != reverseMap.end())
        for(StateSetConstIter iter = mapConstIter->second.begin(); iter != mapConstIter->second.end(); iter++)
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
            for(StateSetIter sIter = preStateSet.begin(); sIter != preStateSet.end(); sIter++)
                if(liveStateSet.find(*sIter) != liveStateSet.end())
                    dynamic_cast<DFAState*>(*sIter)->delDFATrans(*iter);
            set.insert(*iter);
        }
    for(StateSetIter iter = set.begin(); iter != set.end(); iter++)
    {
        stateSet.erase(*iter);
        delete *iter;
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
bool DFA::isReachable(Character character)
{
    if(!initialState)
        return false;
    DFAState* state = dynamic_cast<DFAState*>(initialState);
    state = state->getTargetStateByChar(character) ;
    if(!state) return false;
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
/*  DFA::toNFA                                                     */
/*                                                                 */
/*******************************************************************/
NFA& DFA::nondetermine()const
{
    NFA* nfa = new NFA(*this);
    return *nfa;
}
