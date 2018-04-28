/*****************************************************************************/
/*  FA.hpp                                                                   */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/4/8.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#ifndef FA_hpp
#define FA_hpp

#include <set>
#include <regex>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "CommonUtil.hpp"

using namespace std;
namespace cgh{
    #define EPSILON -1
    class State;
    class FA;
    class NFA;
    class DFA;
    typedef int Character;
    typedef long ID;
    typedef unsigned Flag;
    typedef vector<Character> Word;
    typedef vector<Character> Word;
    typedef unordered_set<FA*> FASet;
    typedef unordered_set<State*> StateSet;
    typedef unordered_set<Character> Alphabet;
    typedef unordered_map<Character, StateSet> NFATansMap;
    typedef unordered_map<Character, State*> DFATansMap;
    typedef unordered_set<FA*>::iterator FASetIter;
    typedef unordered_set<State*>::iterator StateSetIter;
    typedef unordered_set<Character>::iterator AlphabetIter;
    typedef unordered_map<Character, StateSet>::iterator NFATansMapIter;
    typedef unordered_map<Character, State*>::iterator DFATansMapIter;
    typedef pair<NFATansMapIter, bool> NFAMapIter;
    typedef pair<DFATansMapIter, bool> DFAMapIter;
    
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
        void setEpsilonFlag(bool b){flag = b ? (flag | (1<<1)):(flag & ~(1<<1));}
    public:
        ID getID(){return id;}
        bool isFinal(){return (flag & 1) == 1;}
        bool hasEpsilonTrans(){return (flag & (1<<1)) == 1;}
        
        friend FA;
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
                StateSetIter iter;
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
               StateSetIter iter1 = stateSet1.begin();
               StateSetIter iter2 = stateSet2.begin();
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
        iterator begin( void ) { return setMap.begin(); }
        iterator end( void ) { return setMap.end(); }
        iterator find(StateSet stateSet)
        { return setMap.find(stateSet); }
        State* &operator[](StateSet stateSet) { return setMap.operator[](stateSet); }
    };
    
    
    /*****************  class NfaState  *************************************/
    class NFAState : public State
    {
    private:
        NFATansMap nfaTransMap;
        ~NFAState(){}//todo
        
        
    public:
        StateSet getEpsilonClosure();
        StateSet getTargetStateSet();
        StateSet getTargetStateSetByChar(Character character);
        Alphabet getKeySet(){return CommonUtil::getKeySet(nfaTransMap);}
        bool addNFATrans(Character character, NFAState *target);
        bool delNFATrans(Character character, NFAState *target);//todo
        bool delNFATrans(Character character);//todo
        NFAMapIter NFAMapIterator(Character c);
        
    };
    
    
    /*****************  class DfaState  *************************************/
    class DFAState : public State
    {
    private:
        DFATansMap dfaTransMap;
        ~DFAState(){}//todo
       
    public:
        bool addDFATrans(Character character, DFAState *target);//todo
        bool delDFATrans(Character character, DFAState *target);//todo
        bool delDFATrans(Character character);//todo
        
    };
    
    /*****************  class FA  *************************************/
    class FA
    {
    protected:
        State *initialState;
        StateSet stateSet;
        StateSet finalStateSet;
        Alphabet alphabet;
        FA():initialState(NULL){}
        FA(regex regularExecption){}//todo
        FA(FILE *file){}//todo
        virtual ~FA(){}
    public:
        virtual FA &operator &(const FA &fa) = 0;//intersection
        virtual FA &operator |(const FA &fa) = 0;//union
        virtual FA &operator +(const FA &fa) = 0;//concatination
        virtual FA &operator !( void ) = 0;//complement
        virtual FA &getSubFA(State *iState, State *fState) = 0;
        virtual void removeUnreachableState() = 0;
        virtual void removeDeadState() = 0;
        virtual bool isEmpty() = 0;
        virtual Word getOneRun() = 0;
        virtual bool isReachability(Word word) = 0;
        static FA &multiIntersection(FASet &faset);//todo
        static FA &multiConcatination(FASet &faset);//todo
        static FA &multiUnion(FASet &faset);//todo
        static bool multiIntersectionIsEmpty(FASet &faset);//todo
        FA &operator -(FA &fa){return *(this)&(!fa);}//minus
        bool operator ==(FA &fa ){return (*this - fa).isEmpty()&&(fa-*this).isEmpty();}
        bool operator <=(FA &fa ){return (*this - fa).isEmpty();}
    };
    /*****************  class NFA  *************************************/
    class NFA : public FA
    {
    private:
        StateSet getStateSetByChar(StateSet stateSet, Character c);
    public:
        NFA(){}
        ~NFA(){}
        FA &operator &(const FA &fa);
        FA &operator |(const FA &fa);
        FA &operator +(const FA &fa);
        FA &operator !( void );
        FA &getSubFA(State *iState, State *fState);
        void removeUnreachableState();
        void removeDeadState();
        bool isEmpty();
        Word getOneRun();
        bool isReachability(Word word);
        NFAState *mkNFAState();
        NFAState *mkNFAFinalState();
        DFA &toDFA();
        bool hasEpsilon();
        void removeEpsilon();
    };
    
    
    /*****************  class DFA  *************************************/
    class DFA : public FA
    {
    public:
        DFA(){}
        ~DFA(){}
        FA &operator &(const FA &fa);
        FA &operator |(const FA &fa);
        FA &operator +(const FA &fa);
        FA &operator !( void );
        FA &getSubFA(State *iState, State *fState);
        void removeUnreachableState();
        void removeDeadState();
        bool isEmpty();
        Word getOneRun();
        bool isReachability(Word word);
        DFAState *mkDFAState();
        DFAState *mkDFAFinalState();
        DFA &minimize();
        
    };
}


#endif /* FA_hpp */
