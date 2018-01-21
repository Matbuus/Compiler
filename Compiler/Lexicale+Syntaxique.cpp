#include<stdio.h>
#include<ctype.h>
#include <map>
#include <vector>
#include <iostream>
#include <stack>
#include <fstream>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <algorithm>
#include <cstring>
/*
 TO DO : TESTER SUR LA GRAMMAIRE FOURNIE:
 FAIRE UNE ASSOCIATION STRING -> CHAR
 FAIRE UNE ASSOCIATION CHAR -> STRING
 TESTER SUR LE PROGRAMME ECRIT
 */
using namespace std;

// Variables Globales :

string motscles[]={"program","var","begin","end","if","then","else","while","do"};
string types[]={"integer","char"};
string functions[]={"read","readln","write","writeln"};
string oprel[]={"==","<=","<>",">=","<",">"};
string opadd[]={"+","-","||"};
string opmul[]={"*","/","%","&&"};
string symb[]={":=",";",":",",","(",")","(*","*)"};
string ops[]={"(*","*)","(",")","==","<=",">=","<>","<",">","+","-","||","/","%","&&",":=",";",":",",","*"};
string all[]={
    /* Terminaux */
    "(*","*)","(",")","==","<=","<","<>",">",">=",":=","+","-","||","/","%","&&",";",":",",","*","program","var","begin","end","if","then","else","while","do","integer","char","readln","writeln","write","read","id","#","opmul","opadd","oprel","number", //21 + $ = 22
    /* Non Terminaux */
    "LISTE_ID","TYPE","INST_COMPOSEE","LISTE_INST","INST","DCL"
    ,"EXP_SIMPLE","EXP","TERME","FACTEUR","E","V","I","F","D","A","B","P","C" //19
};
int nbProds=0;
bool erreurSyntaxique = false;
vector<string> grammaire;
map <string,char> associationSC;
map <char,string> associationCS;
map<char,string> firsts; // Premiers
map<char,string> follow; //Suivants
map<char,bool> NTer; // NonTerminaux
map<char,bool> visited;
map<char,bool> visitedF;
map< char,map <char,vector<string> > >  TableM; // Table d'analyse
int limit;
vector< string > production; // Les productions de notre grammaire;
map< string, vector< string > > arbreSyntaxiqueS; // Arbre syntaxique avec les chaines originales
map< char, vector< char > > arbreSyntaxiqueC; // Arbre syntaxique avec les caractères associés
vector< string > deriv;
string Axiome = "P";

vector<string> decompos;
enum type{
    entier,caractère,erreur_type
};

typedef struct {
    string nom ;
    string ntype ;
    type vtype ; // 0 : integer   1 : char
    int valeur ;
}tds ;

vector< tds > table_de_symbole;
vector<string> table_des_id ;
string code;
bool danscommentaire = false ;
int nbligne = 1 ;
int nbInsts=0,nbId=1,nbOp=0;
vector<string> apparitionIds; // Ordre des appartitions des IDs ( A utiliser fel semantique )
vector<string> apparitionOprel;
vector<string> apparitionOpmul;
vector<string> apparitionOpadd;
vector<string> arbSyntaxiqueFinal;



// Prototypes :

void findFollow(string&, char ch);
void creerM();
void AfficheFirsts();
void AfficheFollows();
vector<string> verification(string ligne);
void Find_FirstV2(string&, char ch);
void prod(string);
void addProd(string&);
string getOriginal(string);
string getTransformed(string);
void GenGram();
bool isOp(string);
bool containsOp(string);
bool is_key(string, ofstream& file2);
bool is_oprel(string, ofstream& file2);
bool is_symb(string, ofstream& file2);
bool is_type(string, ofstream& file2);
bool is_opadd(string, ofstream& file2);
bool is_opmul(string, ofstream& file2);
bool is_id(string, ofstream& file2);
bool is_number(string, ofstream& file2);
bool is_func(string, ofstream& file2);
void trait(string, ofstream& file2);
void decomp(string mot_lu);
void setType(string s,string ss);
void analyseLexicale(string);
void AfficheProds();
void GenPremiersSuivants();
void AfficheArbreSyntaxique();
void majTypesIds();
void getType(int);
type getTypeid(string);
void controleDeType();




int main()
{
    /* ************************************************************************************************* */
    
    //Generer La grammaire
    GenGram();
    // Fichier contenant le programme a analyser:
    string nomFichier = "/Users/malekattia/Desktop/program_test.txt";
    
    /* ************************************************************************************************* */
    
    /* ANALYSE LEXICALE : */
    
    analyseLexicale(nomFichier);
    // Affichage des productions, des premiers et des suivants et de la table M:
    
    /* FIN ANALYSE LEXICALE */
    
    /* ************************************************************************************************* */
    
    /* ANALYSE SYNTAXIQUE: */
    
    AfficheProds();
    GenPremiersSuivants();
    AfficheFirsts();
    AfficheFollows();
    creerM();
    cout<<endl;
    vector<string> verif = verification(code);
        // Affichage du resultat de l'analyse syntaxique
    for(int i=0;i<verif.size();i++)
        cout<<verif[i];
    
    if(erreurSyntaxique) // Quitter en cas d'erreur syntaxique
        return EXIT_FAILURE;
    
    /* FIN ANALYSE SYNTAXIQUE */
    
    
    /* ************************************************************************************************* */
    
    
    /* ANALYSE SEMANTIQUE */
    AfficheArbreSyntaxique();
    majTypesIds();
    // Affichage des types des vars: VERIF:
    cout<<"\n\n*******\tRESULTAT DE L'ANALYSE SEMANTIQUE\t********\n\n";
    for(int i=0;i<table_de_symbole.size();i++){
        if(table_de_symbole[i].ntype=="id")
            cout<<"ID: "<<table_de_symbole[i].nom<<" a le type "<<table_de_symbole[i].vtype<<endl;
    }
    cout<<"\n\n***************\n\n";
    // TO COMPLETE
    
    controleDeType();
    
    /* FIN ANALYSE SEMANTIQUE */
    
    /* ************************************************************************************************* */
    
    return EXIT_SUCCESS;
    
}

// Affiche les productions de la grammaire :

void AfficheProds(){
    for(int i=0;i<production.size();i++){
        cout<<"Production "<<i<<" :  "<<getOriginal(production[i])<<endl;
    }
}

//Affiche les premiers;

void AfficheFirsts(){
    cout<<"\n\n\n********\tFIRSTs\t********\n\n"<<endl;
    for(map<char,string>::iterator it=firsts.begin();it!=firsts.end();it++){
        /* Parcours de la map qui fait l'association
         entre le caractère et le tableau qui contient ses premiers */
        if(!NTer[it->first]) // On n'affiche pas si c'est un Terminal
            continue;
        cout<<endl;
        cout<<associationCS[it->first]<<" :  "; //{ ";
        for(int i=0;(it->second)[i] != '\0'; i++)
            cout<<associationCS[it->second[i]]<<" ";
        
        //  cout<<" } ";
    }
}

//Affiche les follows;

void AfficheFollows(){
    cout<<"\n\n\n********\tFOLLOWs\t********\n\n"<<endl;
    for(map<char,string>::iterator it=follow.begin();it!=follow.end();it++){
        /* Parcours de la map qui fait l'association
         entre le caractère et le tableau qui contient ses suivants */
        cout<<endl;
        cout<<associationCS[it->first]<<" : ";
        for(int i=0;(it->second)[i] != '\0'; i++){
            if(it->second[i] != '$')
                cout<<associationCS[it->second[i]]<<" ";
            else cout<<it->second[i]<<" ";
        }
    }
}

//ajoute dans une des chaines un element;
/*
 void Array_Manipulation(char array[], char value)
 {
 int temp;
 for(temp = 0; array[temp] != '\0'; temp++)
 {
 if(array[temp] == value)
 {
 return;
 }
 }
 array[temp] = value;
 array[temp + 1] = '\0';
 }
 */

//ajoute dans une des chaines un element;

void Array_ManipulationV2(string &array, char value)
{
    int temp;
    for(temp = 0; temp<array.size(); temp++)
    {
        if(array[temp] == value)
        {
            return;
        }
    }
    array+=value;
}
//Trouve les premiers de la partie droite d'une production/Succession de NT

vector<char> firstProd(string ch){
    vector<char> premiers;
    bool b =false;
    for(int i=0;i<ch.size();i++){ // parcours des elements de la production char par char
        b= false;
        for(int j=0;firsts[ch[i]][j]!='\0';j++){ // parcours des premiers de chaque char
            if(firsts[ch[i]][j]=='#')
                b=true;
            else{
                if(find(premiers.begin(), premiers.end(), firsts[ch[i]][j]) == premiers.end())
                    premiers.push_back(firsts[ch[i]][j]);
            }
        }
        if (b == false) return premiers; // On retourne si epsilon ne fait plus partie des premiers
    }
    premiers.push_back('#');
    /*
     Si on atteint ce niveau, c'est que epsilon (#)
     fait partie des premiers de tous les elements a droite de la production
     On l'ajoute donc au tableau..
     */
    return premiers;
}

// Creation de la table M

void creerM(){ // Algo de classe
    int nbooo = 0;
    // Construction + Affichage de la table d'analyse M :
    cout<<"\n\n\n********\tTABLE M\t********\n\n\n";
    for(int i=0;i<production.size();i++){
        vector<char> premiers = firstProd(production[i].substr(2));
        //Extraire les premiers de alpha (Partie droite de la production ) ds le tableau premiers..
        for(int jj=0;jj<premiers.size();jj++){
            //Parcours des premiers 1 par 1 et on applique l'algo 1)
            if(premiers[jj]!='#'){
                nbooo++;
                cout<<"M [ "<<associationCS[production[i][0]]<<" , "<<associationCS[premiers[jj]]<<" ]  : " << getOriginal(production[i]) << endl;
                TableM[production[i][0]][premiers[jj]].push_back(production[i]);
            }
        }
        if(find(premiers.begin(),premiers.end(),'#') != premiers.end()){
            /*
             Dans le cas ou epsilon fait partie des premiers de alpha..
             On ajoute la production ds tous les M[A,b] tq b appartient
             aux suivants de A
             */
            for(int kk = 0;follow[production[i][0]][kk]!='\0';kk++){
                nbooo++;
                cout<<"M [ "<<associationCS[production[i][0]]<<" , "<<associationCS[follow[production[i][0]][kk]]<<" ]  : " << getOriginal(production[i]) << endl;
                TableM[production[i][0]][follow[production[i][0]][kk]].push_back(production[i]);
            }
        }
    }
    cout<<"\n\n****************\n\n";
}

vector<string> verification(string s){ //algo cours verif qu'un mot est accepté par la Grammaire
    vector<string> messages;
    stack<char> pile;
    string ligne=getTransformed(s);
    int nbRepet = 0,nbRepetMax = code.size();
    int num_ligne_courante=1;
   // int once=0; // Pour n'ajouter la ligne de begin qu'une seule fois TO-FIX
    int numero_id_courant=-1;
    
    // apparitionsID[numero_id_courant] nous renseigne sur quel id on travaille actuellement, a utiliser semantique
    
    ligne+= '$';// On ajoute $ à la fin de la chaîne
    pile.push('$'); // on empile $
    pile.push(production[0][0]); // on empile l'axiome
    int i=0;
    while(true){
        nbRepet++;
        while(pile.top()=='#'){
            pile.pop();
        }
        bool b=false;
        char x = pile.top();
        char a =ligne[i];
        // Mise a jour de la ligne courante
        if(associationCS[a]==";")//A chaque rencontre de ; on inc
            num_ligne_courante++;
        // Fin de Mise a jour de la ligne courante
        
        
        if(x=='$' && a=='$'){
            if(messages.empty())
                messages.push_back("Code sans erreurs syntaxiques.\n");
            else
                messages.push_back("Analyse terminée. Veuillez corriger les erreurs avant de continuer.\n\n");
            return messages;
        }
        else if(NTer[x] == false || x == '$'){
            if(x==a){ // x = a
                pile.pop(); // depiler x
                i++; //avancer ps
            }
            else{ // x!= a
                erreurSyntaxique = true;
                pile.pop();
                string topush="Erreur syntaxique. "+associationCS[x]+" manquant ligne "+to_string(num_ligne_courante)+"\n";
                messages.push_back(topush);
            }
        }
        else if(NTer[x]) { // X est un non terminal
            if(TableM[x][a].size()>0){
                pile.pop();
                for(unsigned long kk=TableM[x][a][0].size()-1;kk>1;kk--){
                    //On empile la partie droite de la production
                    pile.push(TableM[x][a][0][kk]);
                    
                }
                string nv="";
                nv=nv+x+"=>";
                for(unsigned long i = 2 ; i<TableM[x][a][0].size();i++){
                    arbreSyntaxiqueC[x].push_back(TableM[x][a][0][i]);
                    arbreSyntaxiqueS[associationCS[x]].push_back(associationCS[TableM[x][a][0][i]]);
                    nv+=TableM[x][a][0][i];
                    // Calcul du nbr d'id dans la declaration:
                    if(associationCS[TableM[x][a][0][i]]=="id"){
                        numero_id_courant++;
                    }
                }
                arbSyntaxiqueFinal.push_back(nv);
                
                // Mise a jour de la table de symboles :
                
            }
            else{
                // Case vide dans la table..
                erreurSyntaxique = true;
                string erreur ="Erreur syntaxique. Veuillez verifier la syntaxe de "+associationCS[a]+" a la ligne "+to_string(num_ligne_courante)+" ou verifier la presence du \";\" a la ligne "+to_string(num_ligne_courante-1)+" \n";
                if(find(messages.begin(), messages.end(), erreur) == messages.end())
                    messages.push_back(erreur);
                for(int ind = i;ind<ligne.size() && !b;ind++){
                    for(int kk=0;kk<follow[x].size() && !b ;kk++){
                        if(follow[x][kk]==ligne[ind]){
                            i=ind;
                            pile.pop();
                            b=true;
                        }
                    }
                }
            }
        }
        if(nbRepet>nbRepetMax){
            erreurSyntaxique = true;
            return messages;
        }
    }
    return messages;
}

void Find_FirstV2(string& array, char ch) // Algo classe
{
    bool b=true; // verifier que # existe dans les precedents
    if(visitedF[ch]) // condition d'arrêt, eviter redondance
        return;
    visitedF[ch]=true; // ..
    if(NTer[ch]){
        // cout<<"Here with "<< ch<<endl;// verifier que c'est un NT
        for(int i=0;i<limit;i++){ // on parcourt les productions
            if(production[i][0]==ch){
                // ch est dans la partie gauche -> on cherche premiers
                b=true;
                for(int pp=2;production[i][pp]!='\0' ;pp++){ // on parcourt la partie droite
                    if(b){
                        b=false;
                        Find_FirstV2(firsts[production[i][pp]],production[i][pp]); // recursivité et recherche des premiers
                        for(int kk=0;kk<firsts[production[i][pp]].size();kk++){
                            //parcours des premiers trouvés
                            if(firsts[production[i][pp]][kk] != '#'){
                                // on ajoute a l'ensemble des premiers du parent
                                Array_ManipulationV2(firsts[ch], firsts[production[i][pp]][kk]);
                            }
                            else b=true; // # (epsilon existe on peut continuer )
                        }
                    }
                }
            }
        }
        if(b==true)
            Array_ManipulationV2(firsts[ch], '#');
    }
    // Terminal
    else {
        Array_ManipulationV2(firsts[ch], ch); // son premier est lui-même
    }
}


//trouve les Suivants;
void findFollow(string& array, char ch){
    if(visited[ch]) // eviter la redondance
        return;
    visited[ch]=true;
    for(int count=0;count<limit;count++){ // pracours des productions
        for(int j=2;production[count][j]!='\0';j++){
            if(production[count][j]==ch && production[count][j+1]!='\0') // recherche de ch dans la partie droite de la production ( suivants ) + verif qu'il n'est pas en fin de chaine
                //forme (E -> aCHb )
                for(int i=0;i<firsts[production[count][j+1]].size();i++){
                    //On ajoute les prochains privés de epsilon
                    if(firsts[production[count][j+1]][i] != '#')
                        Array_ManipulationV2(follow[ch], firsts[production[count][j+1]][i]);
                    else{
                        //Si epsilon appartient aux premiers, on ajoute les premiers du/des suivant(s)
                        findFollow(follow[production[count][0]], production[count][0]);
                        for(int kk=0;kk<follow[production[count][0]].size();kk++)
                            Array_ManipulationV2(follow[ch], follow[production[count][0]][kk]);
                    }
                }
            else if(production[count][j]==ch){
                // Ch est trouvé en fin de chaîne ( forme E -> aCH )
                //Ajout des suivants du parent
                findFollow(follow[production[count][0]], production[count][0]);
                if(count == 0)
                    Array_ManipulationV2(follow[ch], '$');
                for(int kk=0;kk<follow[production[count][0]].size();kk++)
                    Array_ManipulationV2(follow[ch], follow[production[count][0]][kk]);
            }
        }
    }
}
// Generer les premiers et les suivants:
void GenPremiersSuivants(){
    for(int i=0;i<production.size();i++){
        for(int j=0;j<production[i].length();j++)
            Find_FirstV2(firsts[production[i][j]], production[i][j]); // On remplit les tableaux des premiers pour chaque symbole
    }
    
    for(int i=0;i<limit;i++){
        // On remplit les tableaux des suivants pour chaque symbole
        findFollow(follow[production[i][0]], production[i][0]);
    }
}
//is Op
bool isOp(string mot_lu){
    for(int i=0;i<37;i++){
        if(mot_lu == all[i])
            return true;}
    return false;
}
// verifier que le mot contient un op

bool containsSymb(string mot_lu){
    for(int i=0;i<61;i++){
        if(size_t found = mot_lu.find(all[i]) != string::npos)
            return true;
    }
    return false;
}

// decomposer le mot s'il contient un op

void prod(string mot_lu){
    size_t found;
    while(true){
        for(int i=0;i<59 ;i++){
            //on cherche les symboles 1 par 1
            if((found = mot_lu.find(all[i])) ==0){
                if(mot_lu!=all[i]){
                    mot_lu=mot_lu.substr(all[i].size());
                    grammaire.push_back(all[i]);
                    i=0;
                }
                else {
                    grammaire.push_back(all[i]);
                    return;
                }
            }
        }
    }
}

// AJOUTER CHAQUE PRODUCTION


void addProd(string& mot){
    grammaire.clear(); // Vider le tableau de conversions
    size_t found;
    found=mot.find('=');
    grammaire.push_back(mot.substr(0,found));
    grammaire.push_back("=");
    mot=mot.substr(found+1);
    string ss = "";
    prod(mot);
    char a='0';
    // Fait la transformation Chaines -> Caractères
    for(int i=0;all[i-1]!="C";i++){
        if(a=='#' && all[i]!="#")
            a++;
        else if(all[i]=="#"){
            associationSC[all[i]]='#';
            associationCS['#']=all[i];
        }
        else{
            associationSC[all[i]]=a;
            associationCS[a]=all[i];
        }
        if(i>41)
            NTer[a]=true;
        a++;
    }
    for(int i=0;i<grammaire.size();i++){
        if(i!=1)
            ss+=associationSC[grammaire[i]];
        else ss+=grammaire[i];
    }
    // Ajouter au tableau des productions
    production.push_back(ss);
    nbProds++;
}

// RETOURNE LA CHAINE ORIGINALE
string getOriginal(string production){
    string s = "";
    // Transforme les char en la chaine originale
    s=associationCS[production[0]];
    s+="=";
    for(int i=2;i<production.size();i++)
        s+=associationCS[production[i]];
    
    return s;
}

// RETOURNE LA CHAINE TRANSFORMEE
string getTransformed(string s){
    // Transforme notre chaine en chaine de char
    grammaire.clear();
    string ss = "";
    prod(s);
    for(int i=0;i<grammaire.size();i++)
        ss+=associationSC[grammaire[i]];
    return ss;
    
}

// Generation de la grammaire: A changer pr une autre grammaire:
void GenGram(){
    string mot_lu="";
    mot_lu="P=programid;DCLINST_COMPOSEE";
    addProd(mot_lu);
    mot_lu="DCL=A";
    addProd(mot_lu);
    mot_lu="A=varLISTE_ID:TYPE;A";
    addProd(mot_lu);
    mot_lu="A=#";
    addProd(mot_lu);
    mot_lu="LISTE_ID=idB";
    addProd(mot_lu);
    mot_lu="B=,idB";
    addProd(mot_lu);
    mot_lu="B=#";
    addProd(mot_lu);
    mot_lu="TYPE=integer";
    addProd(mot_lu);
    mot_lu="TYPE=char";
    addProd(mot_lu);
    mot_lu="INST_COMPOSEE=beginINSTend";
    addProd(mot_lu);
    mot_lu="INST=LISTE_INST";
    addProd(mot_lu);
    mot_lu="INST=#";
    addProd(mot_lu);
    mot_lu="LISTE_INST=IV";
    addProd(mot_lu);
    mot_lu="V=;IV";
    addProd(mot_lu);
    mot_lu="V=#";
    addProd(mot_lu);
    mot_lu = "I=id:=EXP_SIMPLE";
    addProd(mot_lu);
    mot_lu="I=ifEXPthenIelseI";
    addProd(mot_lu);
    mot_lu="I=whileEXPdoI";
    addProd(mot_lu);
    mot_lu="I=read(id)";
    addProd(mot_lu);
    mot_lu="I=readln(id)";
    addProd(mot_lu);
    mot_lu="I=write(id)";
    addProd(mot_lu);
    mot_lu="I=writeln(id)";
    addProd(mot_lu);
    mot_lu = "EXP=EXP_SIMPLEF";
    addProd(mot_lu);
    mot_lu = "EXP_SIMPLE=TERMED";
    addProd(mot_lu);
    mot_lu = "D=opaddTERMED";
    addProd(mot_lu);
    mot_lu="D=#";
    addProd(mot_lu);
    mot_lu="TERME=FACTEURE";
    addProd(mot_lu);
    mot_lu="E=opmulFACTEURE";
    addProd(mot_lu);
    mot_lu="E=#";
    addProd(mot_lu);
    mot_lu="FACTEUR=id";
    addProd(mot_lu);
    mot_lu="FACTEUR=number";
    addProd(mot_lu);
    mot_lu="FACTEUR=(EXP_SIMPLE)";
    addProd(mot_lu);
    mot_lu="F=oprelEXP_SIMPLE";
    addProd(mot_lu);
    mot_lu="F=#";
    addProd(mot_lu);
    limit=nbProds;
    // Marquer les non terminaux;
    for(int i=0;i<production.size();i++){
        NTer[production[i][0]]=true; // Pour differencier entre les terminaux et les NT
    }
    // Suivants:
    follow[production[0][0]][0]='$'; // On met un $ pour l'axiome
    follow[production[0][0]][1]='\0'; // rajouter \0
}
/*
 void arbre(string s){
 cout<<s<<" => ";
 for(int i=0;i<arbreSyntaxiqueS[s].size();i++)
 cout<<arbreSyntaxiqueS[s][i]<<" ";
 cout<<endl;
 for(int i=0;i<arbreSyntaxiqueS[s].size();i++)
 if(arbreSyntaxiqueS[arbreSyntaxiqueS[s][i]].size()!=0)
 arbre(arbreSyntaxiqueS[s][i]);
 
 }
 */

// Mise a jour du type dans la table de symbole :

void setType(string s,string ss){
    for(int i=0 ; i<table_de_symbole.size();i++){
        if(table_de_symbole[i].nom==s){
            if(ss=="integer")
                table_de_symbole[i].vtype = entier;
            else
                table_de_symbole[i].vtype = caractère;
        }
        
    }
    
}


// Verifier que l'élément est present dans la table de symboles :
bool check(string s ){
    for(int i=0 ; i<table_de_symbole.size();i++){
        if(table_de_symbole[i].nom==s) return false;
    }
    return true ;
}
// Inserer id dans la table des Id
void insert_id(string s ){
    table_des_id.push_back(s);
}

// Inserer Element dans la table de symboles :

void symb_table_insert(string s , string v){
    if(v=="id"&&!check(s)) return ;
    if(v=="id") insert_id(s);
    //if((v!="id")||(v=="id"&&!check(s))){
    tds element ;
    element.nom = s;
    element.ntype =v;
    element.vtype = erreur_type ;
    element.valeur = 0 ;
    table_de_symbole.push_back(element);
    //}
    return ;
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
        cout<<mot_lu<<endl;
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

bool is_key(string s ,ofstream& file2){
    for(int i = 0 ;i<9;i++){
        if (s == motscles[i]){
            symb_table_insert(s,"keyword");
            file2<<motscles[i];
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un oprel

bool is_oprel(string s,ofstream& file2){
    for(int i = 0 ;i<6;i++){
        if (s == oprel[i]){
            symb_table_insert(s,"oprel");
            file2<<"oprel";
            apparitionOprel.push_back(s);
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un symbole

bool is_symb(string s,ofstream& file2){
    for(int i =0 ; i<6 ; i++){
        if (s == symb[i]){
            if(symb[i]==";")nbligne++ ;
            symb_table_insert(s,"symb");
            file2<<symb[i];
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un nom de type

bool is_type(string s ,ofstream& file2){
    for(int i = 0 ; i<2 ; i++){
        if(s==types[i]){
            symb_table_insert(s,"type");
            file2<<types[i];
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un opadd

bool is_opadd(string s ,ofstream& file2){
    for(int i = 0 ;i<3;i++){
        if (s == opadd[i]){
            symb_table_insert(s,"opadd");
            file2<<"opadd";
            apparitionOpadd.push_back(s);
            return true ;
        }
    }
    return false;
}

// Verifier si le mot est un opmul

bool is_opmul(string s,ofstream& file2){
    for(int i = 0 ;i<4;i++){
        if (s == opmul[i]){
            symb_table_insert(s,"opmul");
            file2<<"opmul";
            apparitionOpmul.push_back(s);
            return true ;
        }
    }
    return false ;
}

// Verifier si le mot est un id

bool is_id(string s,ofstream& file2){
    if((s[0]>='a'&&s[0]<='z')||(s[0]>='A'&&s[0]<='Z')){
        for(int i =1 ; i<s.size();i++){
            if((s[i]<'a'||s[i]>'z')&&(s[i]<'A'||s[i]>'Z')){
                if(s[i]<'0'||s[i]>'9')
                    return false ;
            }
        }
        symb_table_insert(s,"id");
        file2<<"id";
        apparitionIds.push_back(s);
        return true ;
    }
    return false ;
}

// Verifier si le mot est un nombre

bool is_number(string s,ofstream& file2){
    for(int i=0 ; i<s.size();i++){
        if(s[i]<'0'||s[i]>'9'){
            return false ;
        }
    }
    symb_table_insert(s,"number");
    file2<<"number";
    return true ;
}

// Verifier si le mot est une fonction

bool is_func(string s,ofstream& file2){
    for(int i =0 ; i<4;i++){
        if(s== functions[i]){
            symb_table_insert(s,"function");
            file2<<functions[i];
            return true ;
        }
    }
    return false ;
}


//Traitement sur le "mot"

void trait(string mot_lu,ofstream& file2){
    
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
    
    
    if(!is_key(mot_lu,file2)){
        if(!is_type(mot_lu,file2)){
            if(!is_oprel(mot_lu,file2)){
                if(!is_symb(mot_lu,file2)){
                    if(!is_opadd(mot_lu,file2)){
                        if(!is_opmul(mot_lu,file2)){
                            if(!is_func(mot_lu,file2)){
                                if(!is_id(mot_lu,file2)){
                                    if(!is_number(mot_lu,file2)){
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
// l'analyse lexicale:

void analyseLexicale(string nomFichier){
    
    string lexical;
    ifstream file(nomFichier);
    string mot_lu ;
    ofstream file2("/Users/malekattia/Desktop/Compiler/outputLexical.txt");
    while(file>>lexical){
        decompos.clear();
        if(containsOp(lexical) && !isOp(lexical)){
            decomp(lexical);
            for(int i=0;i<decompos.size();i++)
                trait(decompos[i],file2);
        }
        else
            trait(lexical,file2);
    }
    for(int i =0 ; i<table_de_symbole.size();i++){
        cout<<table_de_symbole[i].nom<<" "<<table_de_symbole[i].ntype<<endl;
    }
    cout<<endl<<endl<<"*****\tTABLE DES IDs\t*****\n\n"<<endl;
    for(int i =0 ; i<table_des_id.size();i++){
        cout<<i<<"  "<<table_des_id[i]<<endl;
    }
    cout<<"\n\n****************\n\n\n";
    file2.close();
    ifstream file2read("/Users/malekattia/Desktop/Compiler/outputLexical.txt");
    file2read>>code;
}
// Affichage de l'arbre syntaxique
void AfficheArbreSyntaxique(){
    cout<<"\n\n*****\tARBRE SYNTAXIQUE\t*****\n\n\n";
    for(int i=0;i<arbSyntaxiqueFinal.size();i++){
        for(int j=0;j<arbSyntaxiqueFinal[i].size();j++){
            if(j!=1 && j!=2)
                cout<<" "<<associationCS[arbSyntaxiqueFinal[i][j]];
            else cout<<arbSyntaxiqueFinal[i][j];
        }
    cout<<endl;
    }
    cout<<"\n\n\n*****************\n\n";
}
// Mettre a jour les types des ids dans la table des symboles :

void majTypesIds(){
    int nbi=1;
    bool declarations = false;
    int intermediaire = 1;
    for(int i=0;i<arbSyntaxiqueFinal.size();i++){
        for(int j=0;j<arbSyntaxiqueFinal[i].size();j++){
            if(declarations && associationCS[arbSyntaxiqueFinal[i][j]]=="id"){
                nbi++; // Nombre d'ids par ligne
            }
            char a =arbSyntaxiqueFinal[i][j];
            if(associationCS[a]=="var")
                declarations = true; // Debut des declarations
            
            else if(declarations && (associationCS[a]=="integer" || associationCS[a]=="char")){
                if(associationCS[a] == "char"){
                    for(int i=intermediaire;i<intermediaire+nbi-1;i++)
                        setType(table_des_id[i], "char"); // Donner a chaque ID son type
                }
                else{
                    for(int i=intermediaire;i<intermediaire+nbi-1;i++)
                        setType(table_des_id[i], "integer");// Donner a chaque ID son type
                }
                intermediaire=intermediaire+nbi-1;
                nbi=1;
            }
            else if (declarations && associationCS[a]=="begin")
                declarations = false; // Fin des declarations
            // Fin Maj Table des symboles
        }
    }
}


int indId = 0,indOprel=0,indOpmul=0,indOpadd=0;
vector<string> exp;
vector<string> exporiginale;
void getType(int indice){
    int indiceFinExp=0;
    int nbe=0,nba=0,nbm=0,nbr=0;
    exp.clear();
    for(indiceFinExp=indice;indiceFinExp<arbSyntaxiqueFinal.size();indiceFinExp++){
        if(associationCS[arbSyntaxiqueFinal[indiceFinExp][0]] == "V" || associationCS[arbSyntaxiqueFinal[indiceFinExp][0]] == "I")
            break;
        for(int j=0;j<arbSyntaxiqueFinal[indiceFinExp].size();j++){
            if(associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]=="id" || associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]=="opmul"|| associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]=="oprel"|| associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]=="opadd"|| associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]=="number")
                {
                    
                    if(associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]=="id"){
                        exp.push_back("id");
                        exporiginale.push_back(apparitionIds[indId+nbe]);
                        nbe++;
                    }
                    else if(associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]=="opmul"){
                        exp.push_back("opmul");
                        exporiginale.push_back(apparitionOpmul[indOpmul+nbm]);
                        nbm++;
                    }
                    else if(associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]=="opadd"){
                        exp.push_back(apparitionOpadd[indOpadd+nba]);
                        exporiginale.push_back(apparitionOpadd[indOpadd+nba]);
                        nba++;
                    }
                    else if(associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]=="oprel"){
                        exp.push_back(apparitionOprel[indOprel+nbr]);
                        exporiginale.push_back(apparitionOprel[indOprel+nbr]);
                        nbr++;
                    }
                    else {
                        exp.push_back(associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]);
                        exporiginale.push_back(associationCS[arbSyntaxiqueFinal[indiceFinExp][j]]);
                        
                    }
                    
                    
                    }
                }
        
    }
}

type getTypeid(string ob){
    for(int i=0;i<table_de_symbole.size();i++)
        if(table_de_symbole[i].nom==ob)
            return table_de_symbole[i].vtype;
    
    return erreur_type;
}

void controleDeType(){
    int nbe=-1;
    int typeaverif = -1;
    bool erreur = false,affectation=false;
    for(int i=0;i<arbSyntaxiqueFinal.size();i++){
        affectation=false;
        erreur = false;
        for(int j=3;j<arbSyntaxiqueFinal[i].size();j++){
            // INCREMENTATION DES INDICES ;
            
            if(associationCS[arbSyntaxiqueFinal[i][j]]=="id"){
                indId++; // Pour savoir sur quel id on travaille
            }
            
            if(associationCS[arbSyntaxiqueFinal[i][j]]=="+" || associationCS[arbSyntaxiqueFinal[i][j]]=="-" || associationCS[arbSyntaxiqueFinal[i][j]]=="||"){
                indOpadd++; // Pour savoir sur quel opadd on travaille
            }
            
            if(associationCS[arbSyntaxiqueFinal[i][j]]=="*" || associationCS[arbSyntaxiqueFinal[i][j]]=="/" || associationCS[arbSyntaxiqueFinal[i][j]]=="&&"|| associationCS[arbSyntaxiqueFinal[i][j]]=="%"){
                cout<<associationCS[arbSyntaxiqueFinal[i][j]]<<" -> on inc" <<endl;
                indOpmul++; // Pour savoir sur quel opadd on travaille
            }
            
            if(associationCS[arbSyntaxiqueFinal[i][j]]==">" || associationCS[arbSyntaxiqueFinal[i][j]]=="<" || associationCS[arbSyntaxiqueFinal[i][j]]==">="|| associationCS[arbSyntaxiqueFinal[i][j]]=="<="|| associationCS[arbSyntaxiqueFinal[i][j]]=="<>"|| associationCS[arbSyntaxiqueFinal[i][j]]=="=="){
                indOprel++; // Pour savoir sur quel opadd on travaille
            }
            
            
            // FIN INCREMENTATION DES INDICES ;

            if(associationCS[arbSyntaxiqueFinal[i][j-1]]!="oprel" && associationCS[arbSyntaxiqueFinal[i][j-1]]!="("  && associationCS[arbSyntaxiqueFinal[i][j]]=="EXP_SIMPLE"){
                if(associationCS[arbSyntaxiqueFinal[i][j-1]]==":="){
                    // Si c'est une affectation, le type a verifier est celui de la partie gauche de l'affectation
                    affectation=true;
                    typeaverif=getTypeid(apparitionIds[indId-1]);
                    exporiginale.push_back(apparitionIds[indId-1]);
                    exporiginale.push_back(":=");
                }
                
                if(typeaverif==2){
                    // Erreur de type : Type = 2
                    cout<<"\nErreur de type, la variable "<<apparitionIds[indId-1]<<" est non declarée."<<endl;
                    cout<<"\n***************\n";
                    erreur=true;
                }
                
                // generation de l'expression
                if(!erreur){
                getType(i+1);
                if(!affectation){
                // Si ce n'est pas une affectation, le type a verifier est soit celui du premier id présent dans l'exp
                typeaverif = getTypeid(apparitionIds[indId]);
                // ou "entier" si il y a un nombre entier dans l'expression
                if(find(exp.begin(), exp.end(), "number") != exp.end())
                    typeaverif = 0;
                }
                
                // on parcourt l'expression et on compare chaque id avec le type a verifier
                for(int kkk=0;kkk<exp.size();kkk+=2){
                    nbe++;
                    if(exp[kkk]=="id"){ // On cherche l'id
                        //cout<<" TYPE A VERIF = "<<typeaverif<<" et on a type de "<<apparitionIds[indId+nbe]<<" = "<<getTypeid(apparitionIds[indId+nbe])<<endl;
                        if(getTypeid(apparitionIds[indId+nbe]) == 2){
                             cout<<"\nErreur de type, la variable "<<apparitionIds[indId+nbe]<<" est non declarée."<<endl;
                            cout<<"\n***************\n";
                            erreur=true;
                        }
                        if(getTypeid(apparitionIds[indId+nbe]) != typeaverif){
                            erreur=true;
                            cout<<"\nErreur semantique dans l'expression :"<<endl;
                            for(int l=0;l<exporiginale.size();l++)
                                cout<<exporiginale[l]<<" ";
                            cout<<"\nVerifiez le type de la variable "<<apparitionIds[indId+nbe]<<".\n"<<endl;
                            cout<<"\n***************\n";
                        
                    }
                }
            }
                nbe=-1;
                    /*
                if(!erreur){
                cout<<"\nL'expression :\n";
                for(int l=0;l<exporiginale.size();l++)
                    cout<<exporiginale[l]<<" ";
                cout<<"\nest semantiquement correcte\n";
                }*/
                exporiginale.clear();
                }
         
        } // Fin if EXP
            if(associationCS[arbSyntaxiqueFinal[i][j]]=="writeln" || associationCS[arbSyntaxiqueFinal[i][j]]=="readln" || associationCS[arbSyntaxiqueFinal[i][j]]=="read" || associationCS[arbSyntaxiqueFinal[i][j]]=="write"){
                if(getTypeid(apparitionIds[indId]) ==2){
                    cout<<"\nErreur lors du read/write, la variable "<<apparitionIds[indId]<<" est non déclarée.\n";
                    cout<<"\n***************\n";
                }
            }
    }
}
}
