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
#include <list>
#include <regex>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "CommonUtil.hpp"
#include "Parser.hpp"
#include "State.hpp"
#include "PDS.hpp"


using namespace std;
namespace cgh{
#define EPSILON -1
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

    typedef vector<Character> Word;
    typedef pair<State*, State*> StatePair;
    typedef unordered_set<FA*> FASet;
    typedef unordered_map<Character, State*> DFATransMap;
    typedef unordered_map<Character, StateSet> NFATransMap;
    typedef unordered_map<Character, StatePair> DFAIntersectionMap;
    typedef FASet::iterator FASetIter;
    typedef DFATransMap::iterator DFATransMapIter;
    typedef NFATransMap::iterator NFATransMapIter;
    typedef DFAIntersectionMap::iterator DFAIntersectionMapIter;
    typedef DFATransMap::const_iterator DFATransMapConstIter;
    typedef NFATransMap::const_iterator NFATransMapConstIter;
    typedef DFAIntersectionMap::const_iterator DFAIntersectionMapConstIter;
    typedef pair<NFATransMapIter, bool> NFAMapIter;
    typedef pair<DFATransMapIter, bool> DFAMapIter;
    
    
    /*****************  class NFAState  *************************************/
    class NFAState : public State
    {
    private:
        NFATransMap nfaTransMap;
        ~NFAState(){}//todo
        vector<string> getSMV(int id);
        void getEpsilonClosure(StateSet& epsilonClosure);
        void getTargetStateSetByChar(StateSet& stateSet, Character character);
        void getTargetStateSet(StateSet& stateSet);
    public:
        NFAState(ID i):State(i){}
        NFAState():State(){}
        const StateSet getTargetStateSet();
        const StateSet getTargetStateSetByChar(Character character);
        Alphabet getKeySet(){return CommonUtil::getKeySet(nfaTransMap);}
        NFATransMap& getNFATransMap(){return nfaTransMap;}
        bool addNFATrans(Character character, NFAState *target);
        bool addEpsilonTrans(NFAState *target);
        bool delNFATrans(Character character, NFAState *target);
        bool delNFATrans(Character character);
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
        friend NFA;
    };
    
    
    /*****************  class DFAState  *************************************/
    class DFAState : public State
    {
    private:
        DFATransMap dfaTransMap;
        ~DFAState(){}//todo
        size_t TransMapSize(){return dfaTransMap.size();}
        vector<string> getSMV(int id);
        void getTargetStateSet(StateSet& stateSet);
    public:
        const StateSet getTargetStateSet();
        DFAState* getTargetStateByChar(Character character);
        DFATransMap& getDFATransMap(){return dfaTransMap;}
        bool addDFATrans(Character character, DFAState *target);
        bool delDFATrans(Character character, DFAState *target);
        bool delDFATrans(State *target);
        bool delDFATrans(Character character);
        void output(){
            DFATransMapIter iter;
//            cout<<isFinal()<<endl;
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
        ID id;
        FA():initialState(NULL),flag(0),id(0){}
        FA(regex regularExpression){}//todo
        FA(FILE *file){}//todo
        virtual ~FA(){}
        void setDeterminateFlag(bool b){flag = b ? (flag | 1):(flag & ~1);}
        void setReachableFlag(bool b){flag = b ? (flag | (1<<1)):(flag & ~(1<<1));}
        static vector<vector<string> > getSMV(FASet &faset);
    public:
        ID getID(){return id;}
        void setID(ID i){id = i;}
        bool isDeterminate(){return (flag & 1) == 1;}
        bool isReachable(){return (flag & 1<<1) == (1<<1);}
        bool hasFinalState(const StateSet& stateSet)const;
        void setAlphabet(Alphabet charSet){alphabet.clear(); alphabet.insert(charSet.begin(),charSet.end());}
        void addAlphabet(Alphabet charSet){alphabet.insert(charSet.begin(),charSet.end());}
        void setAlphabet(set<int> charSet){alphabet.clear(); alphabet.insert(charSet.begin(),charSet.end());}
        void addAlphabet(set<int> charSet){alphabet.insert(charSet.begin(),charSet.end());}
        vector<vector<string> > getSMV();
        
        virtual FA &operator &(const FA &fa) = 0;//intersection
        virtual FA &operator |(const FA &fa) = 0;//union
        virtual FA &operator !( void ) = 0;//complement
        
        virtual FA &concat(const FA &fa) = 0;//concatination
        virtual FA &minus(const FA &fa) = 0;//minus
        virtual FA &subset(State *iState, State *fState) = 0;
        virtual DFA &determine() = 0;
        virtual NFA &nondetermine() = 0;
        
        virtual void removeUnreachableState() = 0;
        virtual void removeDeadState() = 0;
        
        virtual Word getOneRun() = 0;
        virtual bool isReachable(Word word) = 0;
        
        virtual void output()const = 0;
        static FA &multiIntersection(FASet &faset);//todo
        static bool multiIntersectionAndDeterminEmptiness(FASet &faset, Alphabet &charSet);//todo
        static bool multiIntersectionAndDeterminEmptiness(list<FASet> &fasetList, FASet &faset, Alphabet &charSet);//todo
        static FA &multiConcatination(FASet &faset);//todo
        static FA &multiUnion(FASet &faset);//todo
        static FA &EmptyFA();//todo
        static FA &CompleteFA(Alphabet charSet);//todo
        bool isEmpty();
        bool operator ==(const FA &fa ){return (*this <= fa) && (const_cast<FA&>(fa) <= *this);}
        bool operator <=(const FA &fa ){return minus(fa).isEmpty();}
//        void resetVisitedFlag()
//        {StateSetIter iter; for(iter=stateSet.begin();iter!=stateSet.end();iter++) (*iter)->setVisitedFlag(0);}
        friend DFA;
        friend NFA;
    };
    /*****************  class NFA  *************************************/
    class NFA : public FA
    {
        typedef pair<NFAState*, Character> StateChar;
        typedef pair<StateChar, Character> StateChar2;
        typedef set<StateChar> StateCharSet;
        typedef set<StateChar2> StateChar2Set;
        typedef unordered_map<Character, StateCharSet> Char2StateCharSetMap;
        typedef unordered_map<Character, StateChar2Set> Char2StateChar2SetMap;
        typedef unordered_map<NFAState*, Char2StateCharSetMap> NeedMap;
        typedef unordered_map<NFAState*, Char2StateChar2SetMap> Need2Map;
        typedef StateCharSet::iterator StateCharSetIter;
        typedef StateChar2Set::iterator StateChar2SetIter;
        typedef NeedMap::iterator NeedMapIter;
        typedef Need2Map::iterator Need2MapIter;
        typedef Char2StateCharSetMap::iterator Char2StateCharSetMapIter;
        typedef Char2StateChar2SetMap::iterator Char2StateChar2SetMapIter;
    private:
        void getTransMapByStateSet(const StateSet& stateSet, NFATransMap& nfaTransmap);
        void makeDFATrans(DFAState* preState, SetMapping& setMapping, const NFATransMap& nfaTransMap, DFA* dfa);
        void makeCopyTransByDFA(DFAState* state, State2Map& state2map);
        void makeCopyTransByNFA(NFAState* state, State2Map& state2map);
        
        bool addNeedMap(NFAState* sState, Character sc, NFAState* tState, Character tc, NeedMap& needMap, Need2Map& need2Map);
        bool addNeed2Map(NFAState* sState, Character sc, NFAState* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map);//prestar
        
        void addPreStarNeedMap(NFAState* sState, Character sc, NFAState* tState, Character tc, NeedMap& needMap, Need2Map& need2Map);//prestar
        void addPostStarNeedMap(NFAState* sState, Character sc, NFAState* tState, Character tc, NeedMap& needMap, Need2Map& need2Map);//poststar
        
        void addPreStarNeed2Map(NFAState* sState, Character sc, NFAState* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map);//prestar
        void addPostStarNeed2Map(NFAState* sState, Character sc, NFAState* tState, Character tc1, Character tc2, NeedMap& needMap, Need2Map& need2Map);//poststar
       
        void addPreStarTrans(NFAState* sState, Character sc, NFAState* tState, Character tc, NeedMap& needMap, Need2Map& need2Map);//prestar
        void addPreStarTrans(NFAState* sState, Character sc, NFAState* tState, NeedMap& needMap, Need2Map& need2Map);//prestar
        
        void addPostStarTrans(NFAState* sState, Character sc, NFAState* tState, Character tc, NeedMap& needMap, Need2Map& need2Map);//poststar
        void addPostStarTrans(NFAState* sState, Character sc, NFAState* tState, NeedMap& needMap, Need2Map& need2Map);//poststar
        
    public:
        NFA(){}
        NFA(RawFaData& data);
        NFA(const NFA& nfa);
        NFA(DFA& dfa);
        ~NFA(){}
        FA &operator &(const FA &fa);
        FA &operator |(const FA &fa);
        FA &operator !( void );
        
        FA &concat(const FA &fa);
        FA &minus(const FA &fa);
        FA &subset(State *iState, State *fState);
        DFA &determine();
        NFA &nondetermine();
        
        void removeUnreachableState();
        void removeDeadState();
        
        Word getOneRun();
        bool isReachable(Word word);
        
        NFAState *mkNFAState();
        NFAState *mkNFAInitialState();
        NFAState *mkNFAFinalState();

        NFA &postStar(const PDS& pds);
        NFA &preStar(const PDS& pds);
        bool hasEpsilon();
        void removeEpsilon();
        
        void output()const{
            StateSetIter iter;
            for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
                dynamic_cast<NFAState*>((*iter))->output();
        }
        friend DFA;
    };
    
    
    /*****************  class DFA  *************************************/
    class DFA : public FA
    {
    private:
        void getTransMapByStatePair(const StatePair& statePair, DFAIntersectionMap& dfaIntersectionMap);
        void makeDFAIntersectionTrans(DFAState* preState, PairMapping& pairMapping, const DFAIntersectionMap& dfaIntersectionMap, DFA* dfa);
        void makeCopyTrans(DFAState* state, State2Map& state2map);
        void makeDFAComplementTrans(DFAState* state, DFAState* trapState, State2Map& state2map, DFA* dfa);
        void getReverseMap(State2StateSetMap& reverseMap);
        void getLiveStateSet(const State2StateSetMap& reverseMap, StateSet& liveStateSet, StateSet& workSet);
        void getReachableStateSet(StateSet& reachableStateSet, StateSet& workSet);
        
    public:
        DFA(){}
        DFA(RawFaData& data);
        DFA(const DFA& dfa);
        ~DFA(){}
        FA &operator &(const FA &fa);
        FA &operator |(const FA &fa);
        FA &operator !( void );
        
        FA &concat(const FA &fa);
        FA &minus(const FA &fa);
        FA &subset(State *iState, State *fState);
        DFA &determine();
        NFA &nondetermine();
        DFA &minimize();
        
        void removeUnreachableState();
        void removeDeadState();
        
        Word getOneRun();
        bool isReachable(Word word);
        
        DFAState *mkDFAState();
        DFAState *mkDFAInitialState();
        DFAState *mkDFAFinalState();
        
        
        void output()const{
//            dynamic_cast<DFAState*>(initialState)->output();
            StateSetIter iter;
            for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
                dynamic_cast<DFAState*>((*iter))->output();
        }
        friend DFAState;
    };
}


#endif /* FA_hpp */

