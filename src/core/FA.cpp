/*****************************************************************************/
/*  FA.cpp                                                                   */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/4/8.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#include "FA.hpp"
using namespace cgh;

bool FA::hasFinalState(const StateSet &stateSet)const
{
    StateSetConstIter iter;
    for(iter = stateSet.begin(); iter != stateSet.end(); iter++)
        if((*iter)->isFinal()) return true;
    return false;
}

/*******************************************************************/
/*                                                                 */
/*  FA::mutipleIntersectionAndDeterminEmptiness                    */
/*                                                                 */
/*******************************************************************/
vector<vector<string> > FA::getSMV(FASet &faset)
{
    vector<string> VAR;
    vector<string> ASSIGH_init;
    vector<string> ASSIGH_next;
    vector<string> ASSIGH_init_next;
    vector<string> INVARSPEC;
    for(FASetIter iter = faset.begin(); iter != faset.end(); iter++)
    {
        vector<vector<string> > smv = (*iter)->getSMV();
        VAR.insert(VAR.end(), smv[0].begin(), smv[0].end());
        ASSIGH_init.insert(ASSIGH_init.end(), smv[1].begin(), smv[1].end());
        ASSIGH_init_next.insert(ASSIGH_init_next.end(), smv[2].begin(), smv[2].end());
        ASSIGH_next.insert(ASSIGH_next.end(), smv[3].begin(), smv[3].end());
        INVARSPEC.insert(INVARSPEC.end(), smv[4].begin(), smv[4].end());
    }
    string INVARSPECstr = "(";
    for(int i = 0; i < INVARSPEC.size(); i++)
        INVARSPECstr += " " + INVARSPEC[i] + " &";
    INVARSPECstr.pop_back();
    INVARSPECstr += ")";
    INVARSPEC.clear();
    INVARSPEC.push_back(INVARSPECstr);
    vector<string> SMVarray[5] = {VAR, ASSIGH_init, ASSIGH_init_next, ASSIGH_next, INVARSPEC};
    vector<vector<string> > SMV(SMVarray, SMVarray + 5);
    return SMV;
}

bool FA::multiIntersectionAndDeterminEmptiness(FASet &faset, Alphabet &charSet)
{
    int num = -1;
    vector<string> VAR;
    vector<string> ASSIGH_init;
    vector<string> ASSIGH_next;
    vector<string> INVARSPEC;
    string endStr = "esac ;";
    for(FASetIter iter = faset.begin(); iter != faset.end(); iter++)
    {
        StateSet stateset = (*iter)->stateSet;
        string iniID = to_string((*iter)->initialState->getID());
        string faStr = "state" + to_string(++num);
        string trapStr = " p" + to_string(num);
        string elseStr = "TRUE : " + trapStr + ";";
        string stateSetString = faStr + " : {";
        string iniStateSetString = "init(" + faStr + ") := s" + iniID + ";";
        string nextString = "next(" + faStr + ") := case";
        string finStr = "(";
        ASSIGH_next.push_back(nextString);
        for(StateSetIter sIter = stateset.begin(); sIter != stateset.end(); sIter++)
        {
            string stateStr = " s" + to_string((*sIter)->getID());
            stateSetString += stateStr + ",";
            vector<string> strVec = (*sIter)->getSMV(num);
            ASSIGH_next.insert(ASSIGH_next.end(), strVec.begin(), strVec.end());
            if((*sIter)->isFinal())
                finStr += " " + faStr + " = " + stateStr + " |";
        }
        finStr.pop_back();
        finStr += ")";
        stateSetString += trapStr + "};";
        VAR.push_back(stateSetString);
        ASSIGH_init.push_back(iniStateSetString);
        ASSIGH_next.push_back(elseStr);
        ASSIGH_next.push_back(endStr);
        INVARSPEC.push_back(finStr);
    }
    for(AlphabetIter iter = charSet.begin(); iter != charSet.end(); iter++)
    {
        string charString = "a" + to_string(*iter) + " : boolean;";
        VAR.push_back(charString);
    }
    string INVARSPECstr = "!(";
    for(int i = 0; i < INVARSPEC.size(); i++)
        INVARSPECstr += " " + INVARSPEC[i] + " &";
    INVARSPECstr.pop_back();
    INVARSPECstr += ");";
    ofstream outFile;
    outFile.open("/Users/iEric/Downloads/tools/nuXmv-1.1.1-Darwin/bin/temp.smv");
    outFile << "MODULE main" << endl;
    outFile << "VAR" << endl;
    for(int i = 0; i < VAR.size(); i++)
        outFile << VAR[i] << endl;
    outFile << "ASSIGN" << endl;
    for(int i = 0; i < ASSIGH_init.size(); i++)
        outFile << ASSIGH_init[i] << endl;
    for(int i = 0; i < ASSIGH_next.size(); i++)
        outFile << ASSIGH_next[i] << endl;
    outFile << "INVARSPEC" << endl;
    outFile << INVARSPECstr << endl;
    outFile.close();
//    FILE* pipe = popen("/Downloads/tools/nuXmv-1.1.1-Darwin/bin/nuXmv -old nuXmv-1.1.1-Darwin/bin/temp.smv", "r");
//    if (!pipe) return "ERROR";
//    char buffer[128];
//    std::string result = "";
//    while(!feof(pipe)) {
//        if(fgets(buffer, 128, pipe) != NULL)
//            result += buffer;
//    }
//    pclose(pipe);
//    cout<<result;
    return true;
}

bool FA::multiIntersectionAndDeterminEmptiness(list<FASet> &fasetList, FASet &faset, Alphabet &charSet)
{
    list<FASet>::iterator iter;
    vector<string> VAR;
    vector<string> ASSIGH_init;
    vector<string> ASSIGH_init_next;
    vector<string> ASSIGH_next;
    vector<string> INVARSPEC;
    for(iter = fasetList.begin(); iter != fasetList.end(); iter++)
    {
        vector<vector<string> > SMVVector = getSMV(*iter);
        for(int i = 0; i < SMVVector[2].size(); i++)
            if(INVARSPEC.size() > 0)
                SMVVector[2][i] = INVARSPEC.back() + " & " + SMVVector[2][i];
        SMVVector[3].insert(++SMVVector[3].begin(), SMVVector[2].begin(), SMVVector[2].end());
        VAR.insert(VAR.end(), SMVVector[0].begin(), SMVVector[0].end());
        ASSIGH_init.insert(ASSIGH_init.end(), SMVVector[1].begin(), SMVVector[1].end());
        ASSIGH_next.insert(ASSIGH_next.end(), SMVVector[3].begin(), SMVVector[3].end());
        INVARSPEC.insert(INVARSPEC.end(), SMVVector[4].begin(), SMVVector[4].end());
    }
    cout << "MODULE main" << endl;
    cout << "VAR" << endl;
    for(int i = 0; i < VAR.size(); i++)
        cout << VAR[i] << endl;
    cout << "ASSIGN" << endl;
    for(int i = 0; i < ASSIGH_init.size(); i++)
        cout << ASSIGH_init[i] << endl;
    for(int i = 0; i < ASSIGH_next.size(); i++)
        cout << ASSIGH_next[i] << endl;
    cout << "INVARSPEC" << endl;
//    cout << INVARSPECstr << endl;
    return 1;
}


/*******************************************************************/
/*                                                                 */
/*  FA::isEmpty                                                    */
/*                                                                 */
/*******************************************************************/
bool FA::isEmpty()
{
    if(!initialState) return true;
    if(!isReachable()) removeUnreachableState();
    if(finalStateSet.size() == 0) return true;
    return false;
}
/*******************************************************************/
/*                                                                 */
/*  FA::makeFA                                                     */
/*                                                                 */
/*******************************************************************/
FA& FA::EmptyFA()
{
    DFA* dfa = new DFA();
    return *dfa;
}
FA& FA::CompleteFA(Alphabet charSet)
{
    DFA* dfa = new DFA();
    DFAState* iniState = dfa->mkDFAFinalState();
    dfa->initialState = iniState;
    dfa->setAlphabet(charSet);
    AlphabetIter iter;
    for(iter = charSet.begin(); iter != charSet.end(); iter++)
        iniState->addDFATrans(*iter, iniState);
    return *dfa;
}

/*******************************************************************/
/*                                                                 */
/*  FA::getSMV                                                    */
/*                                                                 */
/*******************************************************************/

vector<vector<string> > FA::getSMV()
{
    vector<string> VAR;
    vector<string> ASSIGH_init;
    vector<string> ASSIGH_init_next;
    vector<string> ASSIGH_next;
    vector<string> INVARSPEC;
    vector<vector<string> > SMVVector;
    string endStr = "esac ;";
    string iniID = to_string(initialState->getID());
    string faStr = "state" + to_string(id);
    string trapStr = " p" + to_string(id);
    string elseStr = "TRUE : " + trapStr + ";";
    string stateSetString = faStr + " : {";
    string iniStateSetString = "init(" + faStr + ") := s" + iniID + ";";
    string nextString = "next(" + faStr + ") := case";
    string ini2ini = faStr + " = s" + iniID + " : s" + iniID + ";";
    string finStr = "(";
    ASSIGH_next.push_back(nextString);
    ASSIGH_next.push_back(ini2ini);
    for(StateSetIter sIter = stateSet.begin(); sIter != stateSet.end(); sIter++)
    {
        string stateStr = " s" + to_string((*sIter)->getID());
        stateSetString += stateStr + ",";
        vector<string> strVec = (*sIter)->getSMV((int)id);
        if(*sIter != initialState)
            ASSIGH_next.insert(ASSIGH_next.end(), strVec.begin(), strVec.end());
        else
            ASSIGH_init_next.insert(ASSIGH_init_next.end(), strVec.begin(), strVec.end());
        if((*sIter)->isFinal())
            finStr += " " + faStr + " = " + stateStr + " |";
    }
    finStr.pop_back();
    finStr += ")";
    stateSetString += trapStr + "};";
    VAR.push_back(stateSetString);
    ASSIGH_init.push_back(iniStateSetString);
    ASSIGH_next.push_back(elseStr);
    ASSIGH_next.push_back(endStr);
    INVARSPEC.push_back(finStr);
    SMVVector.push_back(VAR);
    SMVVector.push_back(ASSIGH_init);
    SMVVector.push_back(ASSIGH_init_next);
    SMVVector.push_back(ASSIGH_next);
    SMVVector.push_back(INVARSPEC);
    return SMVVector;
}



