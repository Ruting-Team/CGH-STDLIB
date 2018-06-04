//
//  FAState.cpp
//  CGH
//
//  Created by 何锦龙 on 2018/5/7.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#include "FA.hpp"
using namespace cgh;
/*******************************************************************/
/*                                                                 */
/*  NFAState::addNFATrans                                          */
/*                                                                 */
/*******************************************************************/
bool NFAState::addNFATrans(Character character, NFAState *target)
{
    NFATransMapIter mapIt = nfaTransMap.find(character);
    if(mapIt == nfaTransMap.end())
    {
        StateSet stateSet;
        stateSet.insert(target);
        nfaTransMap[character] = stateSet;
        return true;
    }
    else
        return mapIt->second.insert(target).second;
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
    NFATransMapIter mapIt = nfaTransMap.find(character);
    if(mapIt == nfaTransMap.end())
        return false;
    else
        {
            StateSetIter sIter = mapIt->second.find(target);
            if(sIter == mapIt->second.end())
                return false;
            if(mapIt->second.size() == 1)
                nfaTransMap.erase(mapIt);
            else
                mapIt->second.erase(sIter);
            return true;
        }
}
bool NFAState::delNFATrans(State *target)//modified
{
    Alphabet charSet;
    for(NFATransMapIter iter = nfaTransMap.begin(); iter != nfaTransMap.end(); iter++)
        if(iter->second.find(target) != iter->second.end())
        {
            if(iter->second.size() == 1)
                charSet.insert(iter->first);
            else
                iter->second.erase(target);
        }
    for(AlphabetIter iter = charSet.begin(); iter != charSet.end(); iter++)
        nfaTransMap.erase(*iter);
    return true;
}
bool NFAState::delNFATrans(Character character)
{
    NFATransMapIter mapIt = nfaTransMap.find(character);
    if(mapIt == nfaTransMap.end())
        return false;
    nfaTransMap.erase(mapIt);
    return true;
}
/*******************************************************************/
/*                                                                 */
/*  NFAState::getEpsilonClosure                                    */
/*                                                                 */
/*******************************************************************/
void NFAState::getEpsilonClosure(StateSet& epsilonClosure)//modified
{
    NFATransMapIter mapIter = nfaTransMap.find(EPSILON);
    if(mapIter == nfaTransMap.end())
        return;
    StateSet workSet;
    for(StateSetIter iter = mapIter->second.begin(); iter != mapIter->second.end(); iter++)
        if(epsilonClosure.insert(*iter).second) workSet.insert(*iter);
    for(StateSetIter iter = workSet.begin(); iter != workSet.end(); iter++)
        dynamic_cast<NFAState*>(*iter)->getEpsilonClosure(epsilonClosure);
}
/*******************************************************************/
/*                                                                 */
/*  NfaState::getTargetStateSet                                    */
/*                                                                 */
/*******************************************************************/
const StateSet NFAState::getTargetStateSet()
{
    StateSet stateSet;
    StateSet tempSet;
    for(NFATransMapIter iter = nfaTransMap.begin(); iter != nfaTransMap.end(); iter++)
    {
        tempSet.clear();
        getTargetStateSetByChar(tempSet, iter->first);
        if(tempSet.size() > 0)
            stateSet.insert(tempSet.begin(), tempSet.end());
    }
    return stateSet;
}
void NFAState::getTargetStateSet(StateSet& stateSet)
{
    StateSet tempSet;
    for(NFATransMapIter iter = nfaTransMap.begin(); iter != nfaTransMap.end(); iter++)
    {
        tempSet.clear();
        getTargetStateSetByChar(tempSet, iter->first);
        if(tempSet.size() > 0)
            stateSet.insert(tempSet.begin(), tempSet.end());
    }
}
/*******************************************************************/
/*                                                                 */
/*  NfaState::getTargetStateSetByChar                              */
/*                                                                 */
/*******************************************************************/
const StateSet NFAState::getTargetStateSetByChar(Character character)//modified
{
    StateSet epsilonClosure;
    getEpsilonClosure(epsilonClosure);
    if(character == EPSILON) return epsilonClosure;
    epsilonClosure.insert(this);
    StateSet stateSet;
    for(StateSetIter iter = epsilonClosure.begin(); iter != epsilonClosure.end(); iter++)
    {
        NFAState* nfaState = dynamic_cast<NFAState*>(*iter);
        NFATransMapIter mapIter = nfaState->nfaTransMap.find(character);
        if(mapIter != nfaState->nfaTransMap.end())
            for(StateSetIter iter = mapIter->second.begin(); iter != mapIter->second.end(); iter++)
            {
                dynamic_cast<NFAState*>(*iter)->getEpsilonClosure(stateSet);
                stateSet.insert(*iter);
            }
    }
    return stateSet;
}
void NFAState::getTargetStateSetByChar(StateSet& stateSet, Character character)
{
    if(character == EPSILON)
    {
        getEpsilonClosure(stateSet);
        return;
    }
    StateSet epsilonClosure;
    getEpsilonClosure(epsilonClosure);
    epsilonClosure.insert(this);
    for(StateSetIter iter = epsilonClosure.begin(); iter != epsilonClosure.end(); iter++)
    {
        NFAState* nfaState = dynamic_cast<NFAState*>(*iter);
        NFATransMapIter mapIter = nfaState->nfaTransMap.find(character);
        if(mapIter != nfaState->nfaTransMap.end())
            for(StateSetIter iter = mapIter->second.begin(); iter != mapIter->second.end(); iter++)
            {
                dynamic_cast<NFAState*>(*iter)->getEpsilonClosure(stateSet);
                stateSet.insert(*iter);
            }
    }
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
            string stateStr = to_string((*sIter)->getID()) + ";";
            strVector.push_back(str + stateStr);
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
    DFATransMapIter mapIt = dfaTransMap.find(character);
    if(mapIt != dfaTransMap.end() && mapIt->second == target)
    {
        dfaTransMap.erase(mapIt);
        return true;
    }
    return false;
}

bool DFAState::delDFATrans(State *target)//modified
{
    Alphabet charSet;
    for(DFATransMapIter iter = dfaTransMap.begin(); iter != dfaTransMap.end(); iter++)
        if(iter->second == target)
            charSet.insert(iter->first);
    if(charSet.size() == 0) return false;
    for(AlphabetIter iter = charSet.begin(); iter != charSet.end(); iter++)
        dfaTransMap.erase(*iter);
    return true;
}

bool DFAState::delDFATrans(Character character)
{
    DFATransMapIter mapIt = dfaTransMap.find(character);
    if(mapIt == dfaTransMap.end()) return false;
    dfaTransMap.erase(mapIt);
    return true;
}
/*******************************************************************/
/*                                                                 */
/*  DFAState::getTargetStateSet                                    */
/*                                                                 */
/*******************************************************************/
const StateSet DFAState::getTargetStateSet()
{
    StateSet stateSet;
    for(DFATransMapIter iter = dfaTransMap.begin(); iter != dfaTransMap.end(); iter++)
        stateSet.insert(iter->second);
    return stateSet;
}
void DFAState::getTargetStateSet(StateSet &stateSet)
{
    for(DFATransMapIter iter = dfaTransMap.begin(); iter != dfaTransMap.end(); iter++)
        stateSet.insert(iter->second);
}
/*******************************************************************/
/*                                                                 */
/*  DFAState::getTargetStateByChar                                 */
/*                                                                 */
/*******************************************************************/
DFAState* DFAState::getTargetStateByChar(Character character)
{
    DFATransMapIter mapIter = dfaTransMap.find(character);
    if(mapIter != dfaTransMap.end()) return dynamic_cast<DFAState*>(mapIter->second);
    DFAState* null = NULL;
    return null;
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
