//
//  CommonUtil.hpp
//  CGH
//
//  Created by 何锦龙 on 2018/4/9.
//  Copyright © 2018年 何锦龙. All rights reserved.
//

#ifndef CommonUtil_hpp
#define CommonUtil_hpp

#include <set>
#include <regex>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using namespace std;
namespace cgh{
    class CommonUtil
    {
    public:
        static vector<string> split(string str, string s);
        static string getParentheses(string str);
        static vector<vector<string> > getPowerset(vector<string> strvec);
        static vector<vector<int> > Combination(vector<int> num,int len);
        static vector<vector<int> > Permutation(vector<int> num);
        static vector<vector<int> > Permutation_NULL(vector<int> num);
        template<class T1, class T2>
        static unordered_set<T1> getKeySet(unordered_map<T1, T2> map){
            unordered_set<T1> keySet;
            typename std::unordered_map<T1, T2>::iterator iterator;
            for(iterator = map.begin(); iterator != map.end(); iterator++)
                keySet.insert(map);
            return keySet;
        }
        template<class T>
        static unordered_set<T> union_set(unordered_set<T> set1, unordered_set<T> set2){
            unordered_set<T> set(set2.begin(),set2.end());
            typename std::unordered_set<T>::iterator iter=set1.begin();
            for(iter = set1.begin(); iter != set2.end(); iter++)
                set.insert(*iter);
            return set;
        }
        template<class T>
        static unordered_set<T> intersection_set(unordered_set<T> set1, unordered_set<T> set2){
            unordered_set<T> set;
            typename std::unordered_set<T>::iterator iter=set1.begin();
            for(iter = set1.begin(); iter != set1.end(); iter++)
                if(set2.find(*iter))
                    set.insert(*iter);
            return set;
        }
        template <class T>
        static int getArrayLen(T& array){return (sizeof(array) / sizeof(array[0]));}
    };
    
}

#endif /* CommonUtil_hpp */
