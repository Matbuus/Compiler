/*
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std ;

string motscles[]{"program","var","integer","char","begin","end","if","then","else","while","do"};
string types[]{"integer","char"};
string functions[]{"read","readln","write","writeln"};
string oprel[]{"==","<=","<","<>",">",">="};
string opadd[]{"+","-","||"};
string opmul[]{"*","/","%","&&"};
string symb[]{":=",";",":",","};
string ops[]{"(*","*)","==","<=","<","<>",">",">=","+","-","||","/","%","&&",":=",";",":",",","*"};
vector<string> decompos;
bool danscommentaire = false ;

void decomp(string mot_lu);
bool containsOp(string mot_lu){
    for(int i=0;i<19;i++){
        if(size_t found = mot_lu.find(ops[i]) != string::npos){
            return true;
        }
    }
    return false;
}

void decomp(string mot_lu){
    size_t found;
    for(int i=0;i<17;i++){
        if((found = mot_lu.find(ops[i])) != string::npos){
            if(mot_lu.substr(0,found)!= "" && mot_lu.substr(0,found)!= ops[i])
                decompos.push_back(mot_lu.substr(0,found));
            decompos.push_back(ops[i]);
            if(containsOp(mot_lu.substr(found + ops[i].size())))
                decomp(mot_lu.substr(found + ops[i].size()));
            else{
                if(mot_lu.substr(found + ops[i].size())!= "")
                    decompos.push_back(mot_lu.substr(found + ops[i].size()));
                return;
            }
            break;
        }
    }
}*/
