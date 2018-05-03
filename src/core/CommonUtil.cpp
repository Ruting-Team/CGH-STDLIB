//
//  CommonUtil.cpp
//  CGH
//
//  Created by 何锦龙 on 2018/4/9.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#include "CommonUtil.hpp"
vector<string> split(string str, string s)
{
    vector<string> strList;
    int pos=int(str.find(s));
    int ipos=0;
    int slen=int(s.length());
    while(pos!=-1)
    {
        int len=pos-ipos;
        string ss=str.substr(ipos,len);
        if(ss.size()>0)
            strList.push_back(ss);
        ipos=pos+slen;
        pos=int(str.find(s,ipos));
    }
    if(pos==-1)
        strList.push_back(str.substr(ipos,str.length()));
    return strList;
}
string getParentheses(string str)
{
    int i=int(str.find_first_of('('))+1;
    int j=int(str.find_last_of(')'))-i;
    return str.substr(i,j);
}
extern vector<vector<int> > Combination(vector<int> num,int len)
{
    vector<vector<int> > com_vec;
    int n = 1<<len;
    for(int i=1;i<n;i++)    //从 1 循环到 2^len -1
    {
        vector<int> vec;
        for(int j=0;j<len;j++)
        {
            int temp = i;
            if(temp & (1<<j))   //对应位上为1，则输出对应的字符
            {
                vec.push_back(num[j]);
            }
        }
        com_vec.push_back(vec);
    }
    return com_vec;
}
extern vector<vector<int> > Permutation(vector<int> num)
{
    int len=int(num.size());
    vector<vector<int> > permit;
    vector<vector<int> > com=Combination(num, len);
    for(int j=0;j<com.size();j++)
    {
        do
        {
            vector<int> n;
            for(int i=0;i<com[j].size();i++)
            {
                n.push_back(com[j][i]);
            }
            permit.push_back(n);
        }
        while(next_permutation(com[j].begin(), com[j].end()));
    }
    return permit;
}
extern vector<vector<int> > Permutation_NULL(vector<int> num)
{
    vector<vector<int> > permit=Permutation(num);
    vector<vector<int> > nullpermut;
    
    for(int i=0;i<permit.size();i++)
    {
        nullpermut.push_back(permit[i]);
        int n=int(permit[i].size());
        vector<int> vec(n);
        for(int j=0;j<n;j++)
            vec[j]=j*2+1;
        vector<vector<int> > com=Combination(vec, n);
        for(int j=0;j<com.size();j++)
        {
            vector<int> intvec(2*n,100);
            vector<int> intvec0;
            for(int k=0;k<com[j].size();k++)
                intvec[com[j][k]]=0;
            for(int k=0;k<permit[i].size();k++)
                intvec[2*k]=permit[i][k];
            for(int k=0;k<intvec.size();k++)
                if(intvec[k]!=100)
                    intvec0.push_back(intvec[k]);
            nullpermut.push_back(intvec0);
        }
    }
    return nullpermut;
}

