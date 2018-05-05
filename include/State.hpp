/*****************************************************************************/
/*  State.hpp                                                                */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/5/3.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#ifndef State_hpp
#define State_hpp

#include <climits>
#include <set>
#include <list>
#include <regex>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
namespace cgh{
    using namespace std;
    class State;
    class NFAState;
    class NTDState;
    class DTDState;
    class FA;
    class NFA;
    class DFA;
    class DTD;
    class NTD;
    class PDS;
    class TrNFA;
    typedef int Character;
    typedef long ID;
    typedef char Flag;
    typedef pair<State*, State*> StatePair;
    typedef unordered_set<State*> StateSet;
    typedef unordered_set<Character> Alphabet;
    typedef unordered_map<State*, State*> State2Map;
    typedef unordered_map<State*, StateSet> State2StateSetMap;
    typedef StateSet::iterator StateSetIter;
    typedef Alphabet::iterator AlphabetIter;
    typedef State2Map::iterator State2MapIter;
    typedef State2StateSetMap::iterator State2StateSetMapIter;
    typedef StateSet::const_iterator StateSetConstIter;
    typedef Alphabet::const_iterator AlphabetConstIter;
    typedef State2Map::const_iterator State2MapConstIter;
    typedef State2StateSetMap::const_iterator State2StateSetMapConstIter;
    class State
    {
        static long counter;
    protected:
        ID id;
        Flag flag;
        State():id(counter++),flag(0){}
        State(ID id):id(id),flag(0){}
        virtual ~State(){}
        void setFinalFlag(bool b){flag = b ? (flag | 1):(flag & ~1);}
        void setVisitedFlag(bool b){flag = b ? (flag | (1<<1)):(flag & ~(1<<1));}
        void setValidFlag(bool b){flag = b ? (flag | (1<<2)):(flag & ~(1<<2));}
        void setEpsilonFlag(bool b){flag = b ? (flag | (1<<3)):(flag & ~(1<<3));}
        virtual const StateSet getTargetStateSet() = 0;
        virtual vector<string> getSMV(int id) = 0;
    public:
        ID getID(){return id;}
        void setID(ID i){id = i;}
        bool isFinal(){return (flag & 1) == 1;}
        bool isVisited(){return (flag & 1<<1) == (1<<1);}
        bool isValid(){return (flag & (1<<2)) == (1<<2);}
        bool hasEpsilonTrans(){return (flag & (1<<3)) == (1<<3);}
        void setNULL(){id = -1;}
        bool isNULL(){return id == -1;}
        
        friend FA;
        friend NFA;
        friend DFA;
        friend NTD;
        friend DTD;
        friend TrNFA;
    };
    /*****************  class SetMapping  *************************************/
    class SetMapping
    {
    private:
        struct hashf
        {
            size_t operator()(const StateSet stateSet) const
            {
                size_t size = (size_t)(*stateSet.begin());
                StateSetConstIter iter;
                for(iter = stateSet.begin(); iter != stateSet.end(); ++iter)
                    size=size ^ (size_t)(*iter);
                return size;
            }
        };
        struct equalf
        {
            int operator()(const StateSet stateSet1, const StateSet stateSet2) const
            {
                if(stateSet1.size() != stateSet2.size())
                    return 0;
                StateSetConstIter iter1 = stateSet1.begin();
                StateSetConstIter iter2 = stateSet2.begin();
                while(iter1 != stateSet1.end())
                    if(*iter1++ != *iter2++)
                        return 0;
                return 1;
            }
        };
        typedef unordered_map<StateSet, State*, hashf, equalf> SetMap;
        SetMap setMap;
    public:
        typedef SetMap::iterator iterator;
        size_t size( void ) { return setMap.size(); }
        iterator begin( void ) { return setMap.begin(); }
        iterator end( void ) { return setMap.end(); }
        iterator find(StateSet stateSet){ return setMap.find(stateSet); }
        State* &operator[](StateSet stateSet) { return setMap.operator[](stateSet); }
    };
    
    /*****************  class PairMapping  *************************************/
    class PairMapping
    {
    private:
        struct hashf
        {
            size_t operator()(const StatePair statePair) const
            {
                return (size_t)statePair.first ^ (size_t)statePair.second;
            }
        };
        struct equalf
        {
            int operator()(const StatePair statePair1, const StatePair statePair2) const
            {
                if(statePair1.first == statePair2.first && statePair1.second == statePair2.second)
                    return true;
                return false;
            }
        };
        typedef unordered_map<StatePair, State*, hashf, equalf> PairMap;
        PairMap pairMap;
    public:
        typedef PairMap::iterator iterator;
        size_t size( void ) { return pairMap.size(); }
        iterator begin( void ) { return pairMap.begin(); }
        iterator end( void ) { return pairMap.end(); }
        iterator find(StatePair statePair){ return pairMap.find(statePair); }
        State* &operator[](StatePair statePair) { return pairMap.operator[](statePair); }
    };
}

#endif /* State_hpp */
