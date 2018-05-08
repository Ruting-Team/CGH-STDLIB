//
//  NFA.cpp
//  CGH
//
//  Created by 何锦龙 on 2018/5/7.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#include "FA.hpp"
using namespace cgh;
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
/*  NFA::intersection                                              */
/*                                                                 */
/*******************************************************************/
FA &NFA::operator &(const FA &fa)
{
    DFA dfa1 = determine();
    DFA dfa2 = const_cast<FA&>(fa).determine();
    return dfa1 & dfa2;
}

/*******************************************************************/
/*                                                                 */
/*  NFA::union                                                     */
/*                                                                 */
/*******************************************************************/
FA &NFA::operator |(const FA &fa)
{
    if(!initialState) return const_cast<FA&>(fa).nondetermine();
    if(!fa.initialState) return *this;
    NFA *nfa = new NFA();
    nfa->setAlphabet(alphabet);
    nfa->addAlphabet(fa.alphabet);
    NFAState* iniState = nfa->mkNFAInitialState();
    NFAState* state1 = nfa->mkNFAState();
    NFAState* state2 = nfa->mkNFAState();
    State2Map state2Map1;
    State2Map state2Map2;
    NFA tempnfa = const_cast<FA&>(fa).nondetermine();
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
/*  NFA::complement                                                */
/*                                                                 */
/*******************************************************************/
FA &NFA::operator !( void )
{
    return !(determine());
}
/*******************************************************************/
/*                                                                 */
/*  NFA::concatination                                             */
/*                                                                 */
/*******************************************************************/
FA &NFA::concat(const FA &fa)
{
    if(!initialState) return const_cast<FA&>(fa).nondetermine();
    if(!fa.initialState) return *this;
    NFA *nfa = new NFA(*this);
    nfa->addAlphabet(fa.alphabet);
    StateSetIter iter;
    StateSet fStateSet;
    fStateSet.insert(nfa->finalStateSet.begin(), nfa->finalStateSet.end());
    nfa->finalStateSet.clear();
    NFAState* state = nfa->mkNFAState();
    State2Map state2Map;
    NFA tempnfa = const_cast<FA&>(fa).nondetermine();
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
FA &NFA::minus(const FA &fa)
{
    return (determine()) & !(const_cast<FA&>(fa));
}

/*******************************************************************/
/*                                                                 */
/*  NFA::subset                                                    */
/*                                                                 */
/*******************************************************************/
FA &NFA::subset(State *iState, State *fState)
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
bool NFA::isReachable(Word word)
{
    return true;
}

/*******************************************************************/
/*                                                                 */
/*  NFA::toDFA                                                     */
/*                                                                 */
/*******************************************************************/

void NFA::getTransMapByStateSet(const StateSet& stateSet, NFATransMap& nfaTransMap)
{
    NFATransMapIter mapIter;
    for(StateSetIter iter = stateSet.begin(); iter != stateSet.end(); iter++)
    {
        NFATransMap& transMap = dynamic_cast<NFAState*>(*iter)->getNFATransMap();
        StateSet set;
        for(mapIter = transMap.begin(); mapIter != transMap.end(); mapIter++)
            if(mapIter->first != EPSILON)
            {
                set.clear();
                dynamic_cast<NFAState*>(*iter)->getTargetStateSetByChar(set,mapIter->first);
                if(set.size() > 0)
                    nfaTransMap[mapIter->first].insert(set.begin(), set.end());
            }
    }
}

void NFA::makeDFATrans(DFAState* preState, SetMapping &setMapping, const NFATransMap &nfaTransMap, DFA* dfa)
{
    NFATransMap transMap;
    for(NFATransMapConstIter mapIter = nfaTransMap.begin(); mapIter != nfaTransMap.end(); mapIter++)
    {
        SetMapping::iterator setMapIter = setMapping.find(mapIter->second);
        DFAState* postState;
        if(setMapIter == setMapping.end())
        {
            transMap.clear();
            getTransMapByStateSet(mapIter->second, transMap);
            if(hasFinalState(mapIter->second)) postState = dfa->mkDFAFinalState();
            else postState = dfa->mkDFAState();
            setMapping[mapIter->second] = postState;
            makeDFATrans(postState, setMapping, transMap, dfa);
        }
        else postState = dynamic_cast<DFAState*>(setMapping[mapIter->second]);
        preState->addDFATrans(mapIter->first, postState);
//                cout<<preState->getID()<<" "<<mapIter->first<<" "<<postState->getID()<<endl;
//                cout<<postState->isFinal()<<endl;
    }
}

DFA &NFA::determine()
{
    DFA *dfa = new DFA();
    if(!initialState) return *dfa;
    dfa->setAlphabet(alphabet);
    StateSet set;
    dynamic_cast<NFAState*>(initialState)->getEpsilonClosure(set);
    set.insert(initialState);
    SetMapping setMapping;
    NFATransMap nfaTransMap;
    getTransMapByStateSet(set,nfaTransMap);
    NFATransMapIter mapIter;
    DFAState *initialState = dfa->mkDFAInitialState();
    setMapping[set] = initialState;
    makeDFATrans(initialState, setMapping, nfaTransMap, dfa);
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
/*  NFA::toNFA                                                     */
/*                                                                 */
/*******************************************************************/
NFA& NFA::nondetermine()
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
        if((*iter)->getID() != initialState->getID()) state = nfa->mkNFAState();
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
    StateSet stateset;
    tState->getTargetStateSetByChar(stateset, tc);
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
        if((*iter)->getID() != initialState->getID()) state = nfa->mkNFAState();
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
