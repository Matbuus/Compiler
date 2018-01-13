/*#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std ;

//les premiers

typedef struct{
    string symbole;
    vector<string> prems;
}premiers;

// les productions

typedef  struct{
    string symbole;
    vector<vector<string>> prods;
}productions;


/*string motscles[]{"program","var","integer","char","begin","end","if","then","else","while","do"};
string types[]{"integer","char"};
string functions[]{"read","readln","write","writeln"};
string oprel[]{"==","<=","<","<>",">",">="};
string opadd[]{"+","-","||"};
string opmul[]{"*","/","%","&&"};
string symb[]{":=",";",":",","};
string ops[]{"==","<=","<","<>",">",">=","+","-","||","*","/","%","&&",":=",";",":",","};
string nonTer[] {"P", "DCL", "LIST_ID", "TYPE", "INST_COMP", "INST", "LIST_INST", "I", "EXP", "EXP_SIMPLE", "TERME", "FACTEUR" };
string ter[] {"id","==","<=","<","<>",">",">=","+","-","||","*","/","%","&&",":=",";",":",",","$","read","readln","write","writeln","program","var","integer","char","begin","end","if","then","else","while","do"};
*/
/*
vector<premiers> first;
vector<string> positionsTer;
vector<string> positionsNTer;
vector<productions> prods;
string ter[]{"a","b","c","d","e"};

int ind(vector<string> v,string s){
    for(int i=0;i<v.size();i++)
        if(v[i]==s)
            return i;
    return -1;
}

string firsts(string s){
    if(!find(ter, ter+5, s)){
        for(int i=0;i<prods[ind(positionsNTer,s)+5].prods.size();i++){
            first[ind(positionsNTer,s)+5].prems.push_back(firsts(prods[ind(positionsNTer,s)].prods[i][0]));
            
        }
    }
        return s;
}
       


int main(){

    for(int i=0;i<5;i++){
        premiers pi;
        positionsTer.push_back(ter[i]);
        pi.symbole = ter[i];
        pi.prems.push_back(ter[i]);
        first.push_back(pi);
    }
 
    premiers pp;
    
    positionsNTer.push_back("S");
    positionsNTer.push_back("A");
    positionsNTer.push_back("B");
    positionsNTer.push_back("C");
    
    productions p;
    p.symbole = "S";
    pp.symbole = "S";
    first.push_back(pp);
    p.prods[0].push_back("A");
    p.prods[0].push_back("B");
    p.prods[0].push_back("C");
    prods.push_back(p);
    productions p2;
    p2.symbole = "A";
    pp.symbole = "A";
    first.push_back(pp);
    p2.prods[0].push_back("a");
    p2.prods[0].push_back("A");
    p2.prods[1].push_back("$");
    prods.push_back(p2);
    productions p3;
    p3.symbole = "B";
    pp.symbole = "B";
    first.push_back(pp);
    p3.prods[0].push_back("b");
    p3.prods[0].push_back("B");
    p3.prods[1].push_back("c");
    p3.prods[1].push_back("B");
    p3.prods[2].push_back("$");
    prods.push_back(p3);
    productions p4;
    p4.symbole = "C";
    pp.symbole = "C";
    first.push_back(pp);
    p4.prods[0].push_back("d");
    p4.prods[0].push_back("e");
    p4.prods[1].push_back("d");
    p4.prods[1].push_back("a");
    p4.prods[2].push_back("d");
    p4.prods[2].push_back("A");
    p4.prods[3].push_back("$");
    prods.push_back(p4);
    
    for(int i=0;i<first[6].prems.size();i++)
        cout<<first[6].prems[i]<<" ";
    return 0;
}

*/

