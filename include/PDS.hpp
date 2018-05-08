/*****************************************************************************/
/*  PDS.hpp                                                                  */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/4/8.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#ifndef PDS_hpp
#define PDS_hpp

#include <set>
#include <regex>
#include <string>
#include <vector>
#include <list>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "CommonUtil.hpp"
#include "State.hpp"


using namespace std;
namespace cgh{
    class State;
    class PDSState;
    class Configuration;
    class PopConfiguration;
    class ReplaceConfiguration;
    class PushConfiguration;
    class PushNConfiguration;
    class PDSTrans;
    class NFA;
    
    typedef unsigned short Type;
    typedef pair<Character, Character> Char2;
    typedef list<Character> CharN;
    typedef list<PDSTrans*> PDSTransList;
    typedef unordered_set<Configuration*> ConfigurationSet;
    typedef unordered_map<Character, ConfigurationSet> PDSTransMap;
    typedef PDSTransMap::iterator PDSTransMapIter;
    typedef PDSTransList::iterator PDSTransListIter;
    typedef ConfigurationSet::iterator ConfigurationSetIter;
    typedef PDSTransMap::const_iterator PDSTransMapConstIter;
    typedef PDSTransList::const_iterator PDSTransListConstIter;
    typedef ConfigurationSet::const_iterator ConfigurationSetConstIter;
    
    /*****************  class Configuration  *************************************/
    class Configuration
    {
    protected:
        PDSState* state;
        Type type;
    public:
        Type getType()const{return type;}
        PDSState* getState()const{return state;}
        virtual size_t getHash()const = 0;
        bool operator ==(const Configuration &config)const;
        virtual void output() = 0;
    };
    
    /*****************  class PopConfiguration  *************************************/
    class PopConfiguration : public Configuration
    {
        
    public:
        PopConfiguration(){type = 0;}
        PopConfiguration(PDSState* s){state = s; type = 0;}
        size_t getHash()const{return size_t(state);}
        bool operator ==(const Configuration &config)const
        {
            if(config.getType() != 0) return false;
            return state == config.getState();
        }
        void output(){}
    };
    /*****************  class RepalceConfiguration  *************************************/
    class ReplaceConfiguration : public Configuration
    {
    private:
        Character stack;
        void setStack(Character character){stack = character;}
    public:
        ReplaceConfiguration(){type = 1;}
        ReplaceConfiguration(PDSState* s){state = s; type = 1;}
        ReplaceConfiguration(PDSState* s, Character character){state = s; setStack(character); type = 1;}
        const Character &getStack()const{return stack;}
        size_t getHash()const{return size_t(state) ^ size_t(stack);}
        bool operator ==(const Configuration &config)const
        {
            if(config.getType() != 1) return false;
            return state == config.getState() && stack == dynamic_cast<const ReplaceConfiguration&>(config).getStack();
        }
        void output(){}
    };
    /*****************  class PushConfiguration  *************************************/
    class PushConfiguration : public Configuration
    {
    private:
        Char2 stack;
        void setStack(Char2 char2){stack.first = char2.first; stack.second = char2.second;}
    public:
        PushConfiguration(){type = 2;}
        PushConfiguration(PDSState* s){state = s; type = 2;}
        PushConfiguration(PDSState* s, Char2 char2){state = s; setStack(char2); type = 2;}
        const Char2 &getStack()const{return stack;}
        size_t getHash()const{return size_t(state) ^ size_t(stack.first) ^ size_t(stack.second);}
        bool operator ==(const Configuration &config)const
        {
            if(config.getType() != 2) return false;
            const Char2 tempStack = dynamic_cast<const PushConfiguration&>(config).getStack();
            return state == config.getState() && stack.first == tempStack.first && stack.second == tempStack.second;
        }
        void output(){}
    };
    
    /*****************  class PushNConfiguration  *************************************/
    class PushNConfiguration : public Configuration
    {
    private:
        CharN stack;
        void setStack(CharN charN){stack.assign(charN.begin(), charN.end());}
    public:
        PushNConfiguration(){type = 3;}
        PushNConfiguration(PDSState* s){state = s; type = 3;}
        PushNConfiguration(PDSState* s, CharN charN){state = s; setStack(charN); type = 3;}
        const CharN &getStack()const{return stack;}
        size_t getHash()const{return size_t(state);}
        bool operator ==(const Configuration &config)const
        {
            if(config.getType() != 3) return false;
            return state == config.getState();
        }
        void output(){}
    };
    
    /*****************  class PDSTrans  *************************************/
    class PDSState : public State
    {
    private:
        ~PDSState(){}
        vector<string> getSMV(int id){vector<string> strvec; return strvec;}
    public:
        const StateSet getTargetStateSet(){StateSet set; return set;}
    };
    
    /*****************  class PDSTrans  *************************************/
    class PDSTrans
    {
    private:
        Configuration* sourceConfig;
        Configuration* targetConfig;
    public:
        PDSTrans(){}
        PDSTrans(Configuration* sConfig, Configuration* tConfig){sourceConfig = sConfig; targetConfig = tConfig;}
        Configuration* getSourceConfiguration(){return sourceConfig;}
        Configuration* getTargetConfiguration(){return targetConfig;}
        PDSState* getSourceState(){return sourceConfig->getState();}
        PDSState* getTargetState(){return targetConfig->getState();}
    };
    
    
    /*****************  class PDS  *************************************/
    class PDS
    {
    private:
        Alphabet alphabet;
        StateSet stateSet;
        PDSTransList pdsPopTransList;
        PDSTransList pdsReplaceTransList;
        PDSTransList pdsPushTransList;
    public:
        PDS(){}
        PDS(FILE *file);
        PDSState* mkPDSState();
        PDSTrans* mkPDSPopTrans(PDSState* sourceState, Character sc, PDSState* targetState);
        PDSTrans* mkPDSReplaceTrans(PDSState* sourceState, Character sc, PDSState* targetState, Character tc);
        PDSTrans* mkPDSPushTrans(PDSState* sourceState, Character c, PDSState* targetState, Char2& char2);
        void mkPDSPushNTrans(PDSState* sourceState, Character c, PDSState* targetState, CharN& charN);
        friend NFA;
    };
}

#endif /* PDS_hpp */
