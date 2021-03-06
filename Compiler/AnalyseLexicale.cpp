#include <iostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std ;

string motscles[]{"program","var","begin","end","if","then","else","while","do"};
string types[]{"integer","char"};
string functions[]{"read","readln","write","writeln"};
string oprel[]{"==","<=","<","<>",">",">="};
string opadd[]{"+","-","||"};
string opmul[]{"*","/","%","&&"};
string symb[]{":=",";",":",",","(",")"};
string ops[]{"(*","*)","(",")","==","<=","<","<>",">",">=","+","-","||","/","%","&&",":=",";",":",",","*"};
vector<string> decompos;

typedef struct {
    string nom ;
    string ntype ;
    int vtype; // 0 : integer   1 : char
    int valeur;
}tds ;

vector< tds > table_de_symbole;
bool danscommentaire = false ;
int nbligne = 1 ;

bool check(string s ){
    for(int i=0 ; i<table_de_symbole.size();i++){
        if(table_de_symbole[i].nom==s) return false;
    }
    return true ;
}

void symb_table_insert(string s , string v){
    if(v=="id"&&!check(s)) return ;
    //if((v!="id")||(v=="id"&&!check(s))){
    tds element ;
    element.nom = s;
    element.ntype =v;
    element.vtype = -1 ;
    element.valeur = 0 ;
    table_de_symbole.push_back(element);
    //}
    return ;
}


void decomp(string mot_lu);

//is Op
bool isOp(string mot_lu){
    for(int i=0;i<21;i++){
        if(mot_lu == ops[i])
            return true;}
    return false;
}
// verifier que le mot contient un op

bool containsOp(string mot_lu){
    for(int i=0;i<21;i++){
        if(size_t found = mot_lu.find(ops[i]) != string::npos)
            return true;
    }
    return false;
}

// decomposer le mot s'il contient un op

void decomp(string mot_lu){
    size_t found;
    for(int i=0;i<21;i++){
        if((found = mot_lu.find(ops[i])) != string::npos){
            if(mot_lu.substr(0,found)!= "")
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
}

// Verifier si le mot est un mot cl»

bool is_key(string s ){
    for(int i = 0 ;i<9;i++){
        if (s == motscles[i]){
            symb_table_insert(s,"keyword");
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un oprel

bool is_oprel(string s){
    for(int i = 0 ;i<6;i++){
        if (s == motscles[i]){
            symb_table_insert(s,"oprel");
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un symbole

bool is_symb(string s){
    for(int i =0 ; i<6 ; i++){
        if (s == symb[i]){
            if(symb[i]==";")nbligne++ ;
            symb_table_insert(s, "symb");
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un nom de type

bool is_type(string s ){
    for(int i = 0 ; i<2 ; i++){
        if(s==types[i]){
            symb_table_insert(s,"type");
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un opadd

bool is_opadd(string s ){
    for(int i = 0 ;i<3;i++){
        if (s == opadd[i]){
            symb_table_insert(s,"opadd");
            return true ;
        }
    }
    return false;
}

// Verifier si le mot est un opmul

bool is_opmul(string s){
    for(int i = 0 ;i<4;i++){
        if (s == opmul[i]){
            symb_table_insert(s,"opmul");
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un id

bool is_id(string s){
    if((s[0]>='a'&&s[0]<='z')||(s[0]>='A'&&s[0]<='Z')){
        for(int i =1 ; i<s.size();i++){
            if((s[i]<'a'||s[i]>'z')&&(s[i]<'A'||s[i]>'Z')){
                if(s[i]<'0'||s[i]>'9')
                    return false ;
            }
        }
        symb_table_insert(s,"id");
        return true ;
    }
    return false ;
}

// Verifier si le mot est un nombre

bool is_number(string s){
    for(int i=0 ; i<s.size();i++){
        if(s[i]<'0'||s[i]>'9'){
            return false ;
        }
    }
    symb_table_insert(s,"number");
    return true ;
}

// Verifier si le mot est une fonction

bool is_func(string s){
    for(int i =0 ; i<4;i++){
        if(s== functions[i]){
            symb_table_insert(s,"function");
            return true ;
        }
    }
    return false ;
}


//Traitement sur le "mot"

void trait(string mot_lu){

    bool est_nombre = false ;
    bool est_id = false ;
    bool est_motcle = false ;
    bool est_oprel =false ;
    bool est_opadd = false ;
    bool est_opmul = false ;
    bool est_func =false ;
    bool est_type = false ;
    bool est_symb = false ;
    if(danscommentaire&&mot_lu!="*)")return ;
    if(mot_lu=="(*") {
        danscommentaire=true ;
        return;
    }
    if(danscommentaire&&mot_lu=="*)"){
        danscommentaire=false ;
        return;
    }
    if(!danscommentaire&&mot_lu=="*)"){
        cout<<"erreur lexicale "<<mot_lu<<" n'est pas reconnu"<<endl ;
    }

    est_motcle= is_key(mot_lu);
    if(!est_motcle){
        est_type=is_type(mot_lu);
        if(!est_type){
            est_oprel = is_oprel(mot_lu);
            if(!est_oprel){
                est_symb = is_symb(mot_lu);
                if(!est_symb){
                    est_opadd = is_opadd(mot_lu);
                    if(!est_opadd){
                        est_opmul = is_opmul(mot_lu);
                        if(!est_opmul){
                            est_func = is_func(mot_lu);
                            if(!est_func){
                                est_id = is_id(mot_lu);
                                if(!est_id){
                                    est_nombre = is_number(mot_lu);
                                    if(!est_nombre){
                                        cout<<"erreur lexicale dans la ligne "<<nbligne<<" le mot saisi "<<mot_lu<<" n'est pas reconnu"<<endl ;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//Main:


int main(){
    ifstream file("/Users/malekattia/Desktop/program_test.txt");
    string mot_lu ;
    while(file>>mot_lu){
        decompos.clear();
        if(containsOp(mot_lu) && !isOp(mot_lu)){
            decomp(mot_lu);
            for(int i=0;i<decompos.size();i++)
                trait(decompos[i]);
        }
        else
            trait(mot_lu);



    }
    for(int i =0 ; i<table_de_symbole.size();i++){
        cout<<table_de_symbole[i].nom<<" "<<table_de_symbole[i].ntype<<endl;
    }
}


