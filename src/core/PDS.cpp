/*****************************************************************************/
/*  PDS.hpp                                                                   */
/*  CGH                                                                      */
/*  Created by He Jinlong on 2018/4/8.                                       */
/*  Copyright © 2018 Chen Yan, Gao Chong and He Jinlong. All rights reserved.*/
/*****************************************************************************/

#include "PDS.hpp"
using namespace cgh;

/*******************************************************************/
/*                                                                 */
/*  PDS::constructionFromFile                                      */
/*                                                                 */
/*******************************************************************/
PDS::PDS(FILE *file)
{
    
}

/*******************************************************************/
/*                                                                 */
/*  PDS::mkPDSState                                                */
/*                                                                 */
/*******************************************************************/
PDSState* PDS::mkPDSState()
{
    PDSState* state = new PDSState();
    stateSet.insert(state);
    return state;
}

/*******************************************************************/
/*                                                                 */
/*  PDS::mkPDSPopTrans                                             */
/*                                                                 */
/*******************************************************************/
PDSTrans* PDS::mkPDSPopTrans(PDSState* sourceState, Character sc, PDSState* targetState)
{
    Configuration* sourceConfig = new ReplaceConfiguration(sourceState, sc);
    Configuration* targetConfig = new PopConfiguration(targetState);
    PDSTrans* pdsTrans = new PDSTrans(sourceConfig, targetConfig);
    pdsPopTransList.push_back(pdsTrans);
    return pdsTrans;
}
/*******************************************************************/
/*                                                                 */
/*  PDS::mkPDSReplaceTrans                                         */
/*                                                                 */
/*******************************************************************/
PDSTrans* PDS::mkPDSReplaceTrans(PDSState *sourceState, Character sc, PDSState *targetState, Character tc)
{
    Configuration* sourceConfig = new ReplaceConfiguration(sourceState, sc);
    Configuration* targetConfig = new ReplaceConfiguration(targetState, tc);
    PDSTrans* pdsTrans = new PDSTrans(sourceConfig, targetConfig);
    pdsReplaceTransList.push_back(pdsTrans);
    return pdsTrans;
}
/*******************************************************************/
/*                                                                 */
/*  PDS::mkPDSPushTrans                                            */
/*                                                                 */
/*******************************************************************/
PDSTrans* PDS::mkPDSPushTrans(PDSState *sourceState, Character sc, PDSState *targetState, Char2& char2)
{
    Configuration* sourceConfig = new ReplaceConfiguration(sourceState, sc);
    Configuration* targetConfig = new PushConfiguration(targetState, char2);
    PDSTrans* pdsTrans = new PDSTrans(sourceConfig, targetConfig);
    pdsPushTransList.push_back(pdsTrans);
    return pdsTrans;
}

/*******************************************************************/
/*                                                                 */
/*  PDS::mkPDSPushNTrans                                           */
/*                                                                 */
/*******************************************************************/
void PDS::mkPDSPushNTrans(PDSState *sourceState, Character c, PDSState *targetState, CharN& charN)
{
    size_t size = charN.size();
    list<PDSState*> pdsStateList;
    for(int i = 0; i < size - 1; i++)
    {
        PDSState* tempState = sourceState;
        Char2 char2;
        char2.second = charN.back();
        charN.pop_back();
        char2.first = charN.back();
        if(i == size - 2)
            mkPDSPushTrans(tempState, c, targetState, char2);
        else
        {
            PDSState* state = mkPDSState();
            mkPDSPushTrans(tempState, c, state, char2);
            tempState = state;
        }
    }
}

