/*****************************************************************************/
/*  FA.hpp                                                                   */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/4/8.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#ifndef FA_hpp
#define FA_hpp

#include <climits>
#include <set>
#include <regex>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "CommonUtil.hpp"
#include "Parser.hpp"

using namespace std;
namespace cgh{
#define EPSILON INT_MIN
    class State;
    class NFAState;
    class FA;
    class NFA;
    class DFA;
    typedef int Character;
    typedef long ID;
    typedef char Flag;
    typedef vector<Character> Word;
    typedef pair<State*, State*> StatePair;
    typedef unordered_set<FA*> FASet;
    typedef unordered_set<State*> StateSet;
    typedef unordered_set<Character> Alphabet;
    typedef unordered_map<State*, State*> State2Map;
    typedef unordered_map<Character, State*> DFATransMap;
    typedef unordered_map<Character, StateSet> NFATransMap;
    typedef unordered_map<State*, StateSet> State2StateSetMap;
    typedef unordered_map<Character, StatePair> DFAIntersectionMap;
    typedef FASet::iterator FASetIter;
    typedef StateSet::iterator StateSetIter;
    typedef Alphabet::iterator AlphabetIter;
    typedef State2Map::iterator State2MapIter;
    typedef DFATransMap::iterator DFATransMapIter;
    typedef NFATransMap::iterator NFATransMapIter;
    typedef State2StateSetMap::iterator State2StateSetMapIter;
    typedef DFAIntersectionMap::iterator DFAIntersectionMapIter;
    typedef StateSet::const_iterator StateSetConstIter;
    typedef Alphabet::const_iterator AlphabetConstIter;
    typedef DFATransMap::const_iterator DFATransMapConstIter;
    typedef NFATransMap::const_iterator NFATransMapConstIter;
    typedef State2StateSetMap::const_iterator State2StateSetMapConstIter;
    typedef DFAIntersectionMap::const_iterator DFAIntersectionMapConstIter;
    typedef pair<NFATransMapIter, bool> NFAMapIter;
    typedef pair<DFATransMapIter, bool> DFAMapIter;
    
    /*****************  class State  *************************************/
    class State
    {
        static long counter;
    protected:
        ID id;
        Flag flag;
        State():id(counter++),flag(0){}
        virtual ~State(){}
        void setFinalFlag(bool b){flag = b ? (flag | 1):(flag & ~1);}
        void setVisitedFlag(bool b){flag = b ? (flag | (1<<1)):(flag & ~(1<<1));}
        void setValidFlag(bool b){flag = b ? (flag | (1<<2)):(flag & ~(1<<2));}
        void setEpsilonFlag(bool b){flag = b ? (flag | (1<<3)):(flag & ~(1<<3));}
        virtual const StateSet getTargetStateSet() = 0;
    public:
        ID getID(){return id;}
        bool isFinal(){return (flag & 1) == 1;}
        bool isVisited(){return (flag & 1<<1) == (1<<1);}
        bool isValid(){return (flag & (1<<2)) == (1<<2);}
        bool hasEpsilonTrans(){return (flag & (1<<3)) == (1<<3);}
        
        friend FA;
        friend NFA;
        friend DFA;
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
    
    /*****************  class NFAState  *************************************/
    class NFAState : public State
    {
    private:
        NFATransMap nfaTransMap;
        ~NFAState(){}//todo

    public:
        const StateSet getEpsilonClosure();
        const StateSet getTargetStateSet();
        const StateSet getTargetStateSetByChar(Character character);
        Alphabet getKeySet(){return CommonUtil::getKeySet(nfaTransMap);}
        NFATransMap& getNFATransMap(){return nfaTransMap;}
        bool addNFATrans(Character character, NFAState *target);
        bool addEpsilonTrans(NFAState *target);
        bool delNFATrans(Character character, NFAState *target);//todo
        bool delNFATrans(Character character);//todo
        NFAMapIter NFAMapIterator(Character c);
        void output(){
            NFATransMapIter iter;
            for(iter = nfaTransMap.begin(); iter != nfaTransMap.end(); iter++) {
                StateSet set = iter->second;
                StateSetIter sIter;
                for(sIter = set.begin(); sIter != set.end(); sIter++)
                    cout<< getID()<<" "<<iter->first<<" "<<(*sIter)->getID()<<endl;
        }
        }
    };
    
    
    /*****************  class DFAState  *************************************/
    class DFAState : public State
    {
    private:
        DFATransMap dfaTransMap;
        ~DFAState(){}//todo
        size_t TransMapSize(){return dfaTransMap.size();}
    public:
        const StateSet getTargetStateSet();
        DFAState* getTargetStateByChar(Character character);
        DFATransMap& getDFATransMap(){return dfaTransMap;}
        bool addDFATrans(Character character, DFAState *target);//todo
        bool delDFATrans(Character character, DFAState *target);//todo
        bool delDFATrans(State *target);//todo
        bool delDFATrans(Character character);//todo
        void output(){
            DFATransMapIter iter;
            cout<<isFinal()<<endl;
            for(iter = dfaTransMap.begin(); iter != dfaTransMap.end(); iter++) {
                cout<< getID()<<" "<<iter->first<<" "<<iter->second->getID()<<endl;
        }
        }
        friend DFA;
        
    };
    
    /*****************  class FA  *************************************/
    class FA
    {
    protected:
        State *initialState;
        StateSet stateSet;
        StateSet finalStateSet;
        Alphabet alphabet;
        Flag flag;
        FA():initialState(NULL),flag(0){}
        FA(regex regularExpression){}//todo
        FA(FILE *file){}//todo
        virtual ~FA(){}
        void setDeterminateFlag(bool b){flag = b ? (flag | 1):(flag & ~1);}
        void setReachableFlag(bool b){flag = b ? (flag | (1<<1)):(flag & ~(1<<1));}
    public:
        bool isDeterminate(){return (flag & 1) == 1;}
        bool isReachable(){return (flag & 1<<1) == (1<<1);}
        bool hasFinalState(const StateSet& stateSet)const;
        void setAlphabet(Alphabet charSet){alphabet.clear(); alphabet.insert(charSet.begin(),charSet.end());}
        void addAlphabet(Alphabet charSet){alphabet.insert(charSet.begin(),charSet.end());}
        void setAlphabet(set<int> charSet){alphabet.clear(); alphabet.insert(charSet.begin(),charSet.end());}
        void addAlphabet(set<int> charSet){alphabet.insert(charSet.begin(),charSet.end());}
        virtual FA &operator &(const FA &fa) = 0;//intersection
        virtual FA &operator |(const FA &fa) = 0;//union
        virtual FA &operator +(const FA &fa) = 0;//concatination
        virtual FA &operator !( void ) = 0;//complement
        virtual FA &operator -(const FA &fa) = 0;//minus
        virtual DFA &toDFA() = 0;
        virtual NFA &toNFA() = 0;
        virtual FA &getSubFA(State *iState, State *fState) = 0;
        virtual void removeUnreachableState() = 0;
        virtual void removeDeadState() = 0;
        virtual Word getOneRun() = 0;
        virtual bool isReachability(Word word) = 0;
        virtual void output()const = 0;
        static FA &multiIntersection(FASet &faset);//todo
        static FA &multiConcatination(FASet &faset);//todo
        static FA &multiUnion(FASet &faset);//todo
        static FA &EmptyFA();//todo
        static FA &CompleteFA();//todo
        bool isEmpty();
        bool operator ==(const FA &fa ){return (*this <= fa) && (const_cast<FA&>(fa) <= *this);}
        bool operator <=(const FA &fa ){return const_cast<FA&>(*this - fa).isEmpty();}
//        void resetVisitedFlag()
//        {StateSetIter iter; for(iter=stateSet.begin();iter!=stateSet.end();iter++) (*iter)->setVisitedFlag(0);}
        friend DFA;
        friend NFA;
    };
    /*****************  class NFA  *************************************/
    class NFA : public FA
    {
    private:
        const NFATransMap getTransMapByStateSet(const StateSet& stateSet)const;
        void makeDFATrans(DFAState* preState, SetMapping& setMapping, const NFATransMap& nfaTransMap, DFA* dfa)const;
        void makeCopyTransByDFA(DFAState* state, State2Map& state2map);
        void makeCopyTransByNFA(NFAState* state, State2Map& state2map);
    public:
        NFA(){}
        NFA(RawFaData& data);
        NFA(const NFA& nfa);
        NFA(DFA& dfa);
        ~NFA(){}
        FA &operator &(const FA &fa);
        FA &operator |(const FA &fa);
        FA &operator +(const FA &fa);
        FA &operator -(const FA &fa);
        FA &operator !( void );
        FA &getSubFA(State *iState, State *fState);
        void removeUnreachableState();
        void removeDeadState();
        Word getOneRun();
        bool isReachability(Word word);
        NFAState *mkNFAState();
        NFAState *mkNFAInitialState();
        NFAState *mkNFAFinalState();
        DFA &toDFA();
        NFA &toNFA();
        bool hasEpsilon();
        void removeEpsilon();
        void output()const{
            StateSetConstIter iter;
            for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
                dynamic_cast<NFAState*>((*iter))->output();
        }
        friend DFA;
    };
    
    
    /*****************  class DFA  *************************************/
    class DFA : public FA
    {
    private:
        const DFAIntersectionMap getTransMapByStatePair(const StatePair& statePair);
        void makeDFAIntersectionTrans(DFAState* preState, PairMapping& pairMapping, const DFAIntersectionMap& dfaIntersectionMap, DFA* dfa);
        void makeCopyTrans(DFAState* state, State2Map& state2map);
        void makeDFAComplementTrans(DFAState* state, DFAState* trapState, State2Map& state2map, DFA* dfa);
        const StateSet getStateSetByStateSetAndChar(const StateSet &stateSet, Character character);
        const State2StateSetMap getReverseMap();
        const StateSet getRStateSetByStateSetAndRMap(const State2StateSetMap reverseMap, const StateSet stateSet);
        const StateSet getLiveStateSet(const State2StateSetMap reverseMap, const StateSet stateSet);
        const StateSet getReachableStateSet(const StateSet stateSet);
        const StateSet getReachableStateSet();
        
    public:
        DFA(){}
        DFA(RawFaData& data);
        DFA(const DFA& dfa);
        ~DFA(){}
        FA &operator &(const FA &fa);
        FA &operator |(const FA &fa);
        FA &operator +(const FA &fa);
        FA &operator -(const FA &fa);
        FA &operator !( void );
        FA &getSubFA(State *iState, State *fState);
        void removeUnreachableState();
        void removeDeadState();
        Word getOneRun();
        bool isReachability(Word word);
        DFAState *mkDFAState();
        DFAState *mkDFAInitialState();
        DFAState *mkDFAFinalState();
        DFA &minimize();
        DFA &toDFA();
        NFA &toNFA();
        void output()const{
            dynamic_cast<DFAState*>(initialState)->output();
            StateSetConstIter iter;
            for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
                dynamic_cast<DFAState*>((*iter))->output();
        }
        friend DFAState;
    };
}


#endif /* FA_hpp */

