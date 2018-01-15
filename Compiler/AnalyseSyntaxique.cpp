#include<stdio.h>
#include<ctype.h>
#include <map>
#include <vector>
#include <iostream>
#include <stack>
/*
 TO DO : TESTER SUR LA GRAMMAIRE FOURNIE:
 FAIRE UNE ASSOCIATION STRING -> CHAR
 FAIRE UNE ASSOCIATION CHAR -> STRING
 TESTER SUR LE PROGRAMME ECRIT
 */
using namespace std;

// Variables Globales :
string motscles[]{"program","var","begin","end","if","then","else","while","do"};
string types[]{"integer","char"};
string functions[]{"read","readln","write","writeln"};
string oprel[]{"==","<=","<","<>",">",">="};
string opadd[]{"+","-","||"};
string opmul[]{"*","/","%","&&"};
string symb[]{":=",";",":",",","(",")","(*","*)"};
string ops[]{"(*","*)","(",")","==","<=","<","<>",">",">=","+","-","||","/","%","&&",":=",";",":",",","*"};
string all[]{
    /* Terminaux */
    "(*","*)","(",")","==","<=","<","<>",">",">=","+","-","||","/","%","&&",":=",";",":",",","*","program","var","begin","end","if","then","else","while","do","integer","char","readln","writeln","write","read","id","#","9"
    /* Non Terminaux */
    "", "LISTE_ID","TYPE","INST_COMPOSEE","LISTE_INST","INST","DCL"
    ,"EXP_SIMPLE","EXP","TERME","FACTEUR","E","V","I","F","D","A","B","P","C"
};
int nbProds=0;
vector<string> grammaire;
map <string,char> associationSC;
map <char,string> associationCS;
map<char,char[25]> firsts; // Premiers
map<char,char[25]> follow; //Suivants
map<char,bool> NTer; // NonTerminaux
map<char,bool> visited;
map<char,bool> visitedF;
map< char,map <char,vector<string> > >  TableM; // Table d'analyse
int limit;
char production[25][25]; // Les productions de notre grammaire;

// Prototypes :
void Array_Manipulation(char[], char);
void findFollow(char array[], char ch);
void creerM();
void AfficheFirsts();
void AfficheFollows();
string verification(string ligne);
void Find_FirstV2(char array[]  , char ch);
void prod(string);
void addProd(string);
string getOriginal(char[]);
string getTransformed(string);
int main()
{
    // Obsolete: Taper les productions à la main
    /* printf("\nEnter Total Number of Productions:  ");
     scanf("%d", &limit);
     for(count = 0; count < limit; count++)
     {
     printf("\nValue of Production Number [%d]:\t", count + 1);
     scanf("%s", production[count]);
     NTer[production[count][0]] = true;
     }
     */
    
   /*
    Exemples :
    
    Exemple : Automate avec | et &
    strcpy(production[0], "E=TS"); //E' = S
    strcpy(production[1], "S=|TS");
    strcpy(production[2], "S=#"); // # = epsilon
    strcpy(production[3], "T=FQ"); //T' = Q
    strcpy(production[4], "Q=&FQ");
    strcpy(production[5], "Q=#");
    strcpy(production[6], "F=i"); // i = id
    strcpy(production[7], "F=(E)");
    
    Exemple: Classe
    strcpy(production[0], "S=iCtST"); //S' = T
    strcpy(production[1], "T=eS");
    strcpy(production[2], "T=#"); // # = epsilon
    strcpy(production[3], "S=a");
    strcpy(production[4], "C=b");
    limit = 5;
    
    */
    string mot_lu;
    // Exemple : a^n b^n c ; n>1
    mot_lu="P=programid;DCLINST_COMPOSEE";
    addProd(mot_lu);
    /*
    //TEST
    mot_lu="INST_COMPOSEE=end";
    addProd(mot_lu);
    //END TEST
     */
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
    mot_lu = "D=+TERMED";
    addProd(mot_lu);
    mot_lu="D=#";
    addProd(mot_lu);
    mot_lu="TERME=FACTEURE";
    addProd(mot_lu);
    mot_lu="E=*FACTEURE";
    addProd(mot_lu);
    mot_lu="E=#";
    addProd(mot_lu);
    mot_lu="FACTEUR=id";
    addProd(mot_lu);
    mot_lu="FACTEUR=(EXP_SIMPLE)";
    addProd(mot_lu);
    mot_lu="F===EXP_SIMPLE";
    addProd(mot_lu);
    mot_lu="F=#";
    addProd(mot_lu);
    
    limit=nbProds;
    /*
    strcpy(production[0], "Z=XY#");
    strcpy(production[1], "X=aT");
    strcpy(production[2], "T=b");
    strcpy(production[3], "T=Xb");
    strcpy(production[4], "Y=c");
    limit = 5;
    */
    for(int i=0;production[i][0]!='\0';i++){
        cout<<"Production "<<i<<" :  "<<getOriginal(production[i])<<endl;
    }
    for(int i=0;production[i][0]!='\0';i++){
        NTer[production[i][0]]=true; // Pour differencier entre les terminaux et les NT
    }
    
    // TO-DO: Tableau contenant immediatement les T et les NT;
    
    for(int i=0;production[i][0]!='\0';i++){
        for(int j=0;production[i][j]!='\0';j++)
            Find_FirstV2(firsts[production[i][j]], production[i][j]); // On remplit les tableaux des premiers pour chaque symbole
        
    }
    follow[production[0][0]][0]='$'; // On met un $ pour l'axiome
    follow[production[0][0]][1]='\0'; // rajouter \0
    for(int i=0;i<limit;i++){
        // On remplit les tableaux des suivants pour chaque symbole
        findFollow(follow[production[i][0]], production[i][0]);
    }
    AfficheFirsts();
    AfficheFollows();
    creerM();
    cout<<endl;
    cout<<verification("program;varid,id:char;varid,id:integer;beginreadln(id);readln(id);id:=id+id;whileiddowriteln(id);readln(id)end");
    return 0;
}

//Affiche les premiers;

void AfficheFirsts(){
    cout<<"\nFIRSTs :"<<endl;
    for(map<char,char[25]>::iterator it=firsts.begin();it!=firsts.end();it++){
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
    cout<<"\n\nFOLLOWs :"<<endl;
    for(map<char,char[25]>::iterator it=follow.begin();it!=follow.end();it++){
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

//Trouve les premiers de la partie droite d'une production/Succession de NT

vector<char> firstProd(char* ch){
    vector<char> premiers;
    bool b =false;
    for(int i=0;ch[i]!='\0';i++){ // parcours des elements de la production char par char
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
    // Construction + Affichage de la table d'analyse M :
    cout<<"\n\n\n TABLE M : \n\n\n";
    for(int i=0;production[i][0]!='\0';i++){
        vector<char> premiers = firstProd(production[i]+2);
        //Extraire les premiers de alpha (Partie droite de la production ) ds le tableau premiers..
        for(int jj=0;jj<premiers.size();jj++){
            //Parcours des premiers 1 par 1 et on applique l'algo 1)
            if(premiers[jj]!='#'){
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
                cout<<"M [ "<<associationCS[production[i][0]]<<" , "<<associationCS[follow[production[i][0]][kk]]<<" ]  : " << getOriginal(production[i]) << endl;
                TableM[production[i][0]][follow[production[i][0]][kk]].push_back(production[i]);
            }
        }
    }
}

string verification(string s){ //algo cours verif qu'un mot est accepté par la Grammaire
    stack<char> pile;
    string ligne=getTransformed(s);
    ligne+= '$';// On ajoute $ à la fin de la chaîne
    pile.push('$'); // on empile $
    pile.push(production[0][0]); // on empile l'axiome
    int i=0;
    while(true){
        while(pile.top()=='#'){
            pile.pop();
        }
        char x = pile.top();
        char a =ligne[i];
        if(x=='$' && a=='$'){
            return s+ " : Mot Bien Accepté.\n";
        }
        if(NTer[x] == false || x == '$'){
            if(x==a){ // x = a
                pile.pop(); // depiler x
                i++; //avancer ps
            }
            else{ // x!= a
                cout<<"X = "<<associationCS[x]<<endl;
                return "Erreur. Veuillez verifier la position de "+associationCS[a]+"\n";
            }
        }
        else if(NTer[x]) { // X est un non terminal
            if(TableM[x][a].size()>0){
                pile.pop();
                for(unsigned long kk=TableM[x][a][0].size()-1;kk>1;kk--){
                   //On empile la partie droite de la production
                    pile.push(TableM[x][a][0][kk]);
                }
            }
            else{
                // Case vide dans la table..
                 cout<<"X = "<<associationCS[x]<<endl;
                return "Erreur. Veuillez verifier la position de "+associationCS[a]+"\n";
            }
        }
    }
}

void Find_FirstV2(char array[25], char ch) // Algo classe
{
    bool b=true; // verifier que # existe dans les precedents
    if(visitedF[ch]) // condition d'arrêt, eviter redondance
        return;
    visitedF[ch]=true; // ..
    if(NTer[ch]){
       // cout<<"Here with "<< ch<<endl;// verifier que c'est un NT
        for(int i=0;i<limit;i++){ // on parcourt les productions
            if(production[i][0]==ch){ // ch est dans la partie gauche -> on cherche premiers
                b=true;
                for(int pp=2;production[i][pp]!='\0' ;pp++){ // on parcourt la partie droite
                    if(b){
                        b=false;
                        Find_FirstV2(firsts[production[i][pp]],production[i][pp]); // recursivité et recherche des premiers
                        for(int kk=0;firsts[production[i][pp]][kk]!='\0';kk++){
                            //parcours des premiers trouvés
                            if(firsts[production[i][pp]][kk] != '#'){
                                // on ajoute a l'ensemble des premiers du parent
                                Array_Manipulation(firsts[ch], firsts[production[i][pp]][kk]);
                            }
                            else b=true; // # (epsilon existe on peut continuer )
                        }
                    }
                }
            }
        }
        if(b==true)
            Array_Manipulation(firsts[ch], '#');
    }
    // Terminal
    else {
        Array_Manipulation(firsts[ch], ch); // son premier est lui-même
    }
}


//trouve les Suivants;
void findFollow(char array[25], char ch){
    if(visited[ch]) // eviter la redondance
        return;
    visited[ch]=true;
    for(int count=0;count<limit;count++){ // pracours des productions
        for(int j=2;production[count][j]!='\0';j++){
            if(production[count][j]==ch && production[count][j+1]!='\0') // recherche de ch dans la partie droite de la production ( suivants ) + verif qu'il n'est pas en fin de chaine
                //forme (E -> aCHb )
                for(int i=0;firsts[production[count][j+1]][i]!='\0';i++){
                    //On ajoute les prochains privés de epsilon
                    if(firsts[production[count][j+1]][i] != '#')
                        Array_Manipulation(follow[ch], firsts[production[count][j+1]][i]);
                    else{
                        //Si epsilon appartient aux premiers, on ajoute les premiers du/des suivant(s)
                        findFollow(follow[production[count][0]], production[count][0]);
                        for(int kk=0;follow[production[count][0]][kk]!='\0';kk++)
                            Array_Manipulation(follow[ch], follow[production[count][0]][kk]);
                    }
                }
            else if(production[count][j]==ch){
                // Ch est trouvé en fin de chaîne ( forme E -> aCH )
                //Ajout des suivants du parent
                findFollow(follow[production[count][0]], production[count][0]);
                for(int kk=0;follow[production[count][0]][kk]!='\0';kk++)
                    Array_Manipulation(follow[ch], follow[production[count][0]][kk]);
            }
        }
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
    for(int i=0;i<37;i++){
        if(size_t found = mot_lu.find(all[i]) != string::npos)
            return true;
    }
    return false;
}

// decomposer le mot s'il contient un op

void prod(string mot_lu){
    size_t found;
    while(true){
        for(int i=0;i<56;i++){
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


void addProd(string mot){
    grammaire.clear(); // Vider le tableau de conversions
    size_t found;
    found=mot.find('=');
    grammaire.push_back(mot.substr(0,found));
    grammaire.push_back("=");
    mot=mot.substr(found+1);
    string ss = "";
    prod(mot);
    char a='0';
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
        if(i>38)
            NTer[a]=true;
        a++;
    }
    for(int i=0;i<grammaire.size();i++){
        if(i!=1)
            ss+=associationSC[grammaire[i]];
        else ss+=grammaire[i];
    }
    strcpy(production[nbProds],ss.c_str());
    nbProds++;
}

// RETOURNE LA CHAINE ORIGINALE
string getOriginal(char production[]){
    string s = "";
    // Transforme les char en la chaine originale
    s=associationCS[production[0]];
    s+="=";
    for(int i=2;production[i]!='\0';i++)
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

