#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <exception>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr), m_syntaxError(false) {
}

void Interpreteur::analyse() {
    m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
    static char messageWhat[256];
    if (m_lecteur.getSymbole() != symboleAttendu) {
        sprintf(messageWhat,
                "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
                m_lecteur.getLigne(), m_lecteur.getColonne(),
                symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
        throw SyntaxeException(messageWhat);
    }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
    tester(symboleAttendu);
    m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
    // Lève une exception contenant le message et le symbole courant trouvé
    // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
    static char messageWhat[256];
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    testerEtAvancer("procedure");
    testerEtAvancer("principale");
    testerEtAvancer("(");
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finproc");
    tester("<FINDEFICHIER>");
    if (m_syntaxError) return nullptr;
    return sequence;
}

Noeud* Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    NoeudSeqInst* sequence = new NoeudSeqInst();
    do {
        sequence->ajoute(inst());
    } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "lire");
    // Tant que le symbole courant est un début possible d'instruction...
    // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
    return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        Noeud* affect;
        try {
        affect = affectation();
        testerEtAvancer(";");
        } catch (SyntaxeException & e) {
            m_syntaxError = true;
            printf("%s\n",e.what());
            while (m_lecteur.getSymbole() != "si" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole()!="<FINDEFICHIER>") {
                
                m_lecteur.avancer();
                cout << m_lecteur.getSymbole() << endl;
            }
        }
        return affect;
    } else if (m_lecteur.getSymbole() == "si") {
        Noeud* inst;
        try {
            inst = instSi();
        } catch (SyntaxeException & e){
            m_syntaxError = true;
            printf("%s\n",e.what());
            while (m_lecteur.getSymbole()!="<VARIABLE>" && m_lecteur.getSymbole() != "si" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole()!="<FINDEFICHIER>"){
                cout << m_lecteur.getSymbole() << endl;
                m_lecteur.avancer(); 
            }
        }
        if (m_syntaxError) return nullptr;
        return inst;
    }

    else if (m_lecteur.getSymbole() == "tantque") {
        Noeud* inst;
        try {
            inst = instTantQue();
        } catch (SyntaxeException & e){
            m_syntaxError = true;
            printf("%s\n",e.what());
            while (m_lecteur.getSymbole()!="<VARIABLE>" && m_lecteur.getSymbole() != "si" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole()!="<FINDEFICHIER>"){
               m_lecteur.avancer(); 
            }
        }
        if (m_syntaxError) return nullptr;
        return inst;
    }

    else if (m_lecteur.getSymbole() == "repeter") {
        Noeud* inst;
        try {
            inst = instRepeter();
        } catch (SyntaxeException & e){
            m_syntaxError = true;
            printf("%s\n",e.what());
            while (m_lecteur.getSymbole()!="<VARIABLE>" && m_lecteur.getSymbole() != "si" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole()!="<FINDEFICHIER>"){
               m_lecteur.avancer(); 
            }
        }
        if (m_syntaxError) return nullptr;
        return inst;
    }
    else if (m_lecteur.getSymbole() == "pour"){
        Noeud* inst;
        try {
            inst = instPour();
        } catch (SyntaxeException & e){
            m_syntaxError = true;
            printf("%s\n",e.what());
            while (m_lecteur.getSymbole()!="<VARIABLE>" && m_lecteur.getSymbole() != "si" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole()!="<FINDEFICHIER>"){
               m_lecteur.avancer(); 
            }
        }
        if (m_syntaxError) return nullptr;
        return inst;
    }
    else if (m_lecteur.getSymbole() == "ecrire"){
        Noeud* inst;
        try {
            inst = instEcrire();
        } catch (SyntaxeException & e){
            m_syntaxError = true;
            printf("%s\n",e.what());
            while (m_lecteur.getSymbole()!="<VARIABLE>" && m_lecteur.getSymbole() != "si" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole()!="<FINDEFICHIER>"){
               m_lecteur.avancer(); 
            }
        }
        if (m_syntaxError) return nullptr;
        return inst;
    }
    else if (m_lecteur.getSymbole() == "lire") {
        Noeud* inst;
        try {
            inst = instLire();
        } catch (SyntaxeException & e){
            m_syntaxError = true;
            printf("%s\n",e.what());
            while (m_lecteur.getSymbole()!="<VARIABLE>" && m_lecteur.getSymbole() != "si" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole()!="<FINDEFICHIER>"){
               m_lecteur.avancer(); 
            }
        }
        if (m_syntaxError) return nullptr;
        return inst;
    }
        // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
    else erreur("Instruction incorrecte");
}

Noeud* Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression> 
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
    m_lecteur.avancer();
    testerEtAvancer("=");
    Noeud* exp = expression(); // On mémorise l'expression trouvée
    return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
    // <expression> ::= <facteur> { <opBinaire> <facteur> }
    //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud* fact = facteur();
    while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-" ||
            m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/" ||
            m_lecteur.getSymbole() == "<" || m_lecteur.getSymbole() == "<=" ||
            m_lecteur.getSymbole() == ">" || m_lecteur.getSymbole() == ">=" ||
            m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
            m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou"){
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* factDroit = facteur(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
    // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
    Noeud* fact = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
    } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
    } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
        m_lecteur.avancer();
        fact = expression();
        testerEtAvancer(")");
    } else
        erreur("Facteur incorrect");
    return fact;
}

Noeud* Interpreteur::instSi() {
    // <instSi> ::= si ( <expression> ) <seqInst> finsi
    testerEtAvancer("si");
    testerEtAvancer("(");
    Noeud* condition = expression(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); // On mémorise la séquence d'instruction
    testerEtAvancer("finsi");
    return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instTantQue() {
    //<instTantQue> ::= tant que (<expression>) <seqInst> fintantque   
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);
    //return nullptr ; 

}

Noeud* Interpreteur::instRepeter() {
    //<instRepeter> ::= repeter <seqInst> jusqua ( <expression> )
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    return new NoeudInstRepeter(sequence, condition);
    //return nullptr ;

}

Noeud* Interpreteur::instPour() {
    //<instPour> ::= pour ( [ <affectation> ] ; <expression> ; [ <affectation> ] ) <seqInst> finpour
    testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud* initialisation;
    Noeud* iteration;
    try {
        initialisation = affectation();
    } catch (const exception e) {
        initialisation = nullptr;
    }
    /*if (m_lecteur.getSymbole() == "<VARIABLE>"){
    
      initialisation = affectation() ;   //variable 
    }*/
    testerEtAvancer(";");
    Noeud* condition = expression();
    testerEtAvancer(";");
    /*if(m_lecteur.getSymbole() == "<VARIABLE>"){
       iteration = affectation (); //incrementation ou décrémentation
    }*/

    try {
        iteration = affectation();
    } catch (const exception e) {
        iteration = nullptr;
    }
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finpour");
    return new NoeudInstPour(initialisation, condition, iteration, sequence);
    //return nullptr; 
}

Noeud* Interpreteur::instEcrire() {
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    
    NoeudInstEcrire* expV = new NoeudInstEcrire();
    Noeud* chaine = nullptr;
    Noeud* exp = nullptr ; 

    if (m_lecteur.getSymbole() == "<CHAINE>") {
        chaine = m_table.chercheAjoute(m_lecteur.getSymbole());
        expV->ajouter(chaine);
        m_lecteur.avancer();
    } else {
        exp = expression() ; 
        expV->ajouter(exp) ; 
              
    }
    while (m_lecteur.getSymbole() == ","){
        m_lecteur.avancer();
        if (m_lecteur.getSymbole() == "<CHAINE>") {
            chaine = m_table.chercheAjoute(m_lecteur.getSymbole());
            expV->ajouter(chaine);
            m_lecteur.avancer();
        } else {
           exp = expression() ; 
           expV->ajouter(exp) ;
        }
         //m_lecteur.avancer();
    }
    testerEtAvancer(")"); 
    return expV ; 
}

Noeud* Interpreteur::instLire() {
    testerEtAvancer("lire");
    testerEtAvancer("(");
    
    Noeud* variable;
    NoeudInstLire* variableV = new NoeudInstLire();
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
        variable = m_table.chercheAjoute(m_lecteur.getSymbole());
        variableV->ajouter(variable);
        m_lecteur.avancer();
    }
    while (m_lecteur.getSymbole() == ","){
        m_lecteur.avancer();
        if (m_lecteur.getSymbole() == "<VARIABLE>") {
            variable = m_table.chercheAjoute(m_lecteur.getSymbole());
            variableV->ajouter(variable);
            m_lecteur.avancer();
        }
    }
    testerEtAvancer(")");
    return variableV;
}

void Interpreteur::traduitEnCPP(ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation) << " " << "#include <iostream>" << endl << "using namespace std" << endl;
    cout << setw(4*indentation) << " " << "int main() {" << endl;
    vector<SymboleValue*> table = getTable().getTable();
    for (SymboleValue* s : table){
        if (*s=="<VARIABLE>") cout << "int " << s->getChaine() << "; ";
    }
    cout << endl;
    getArbre()->traduitEnCPP(cout, indentation+1);
    cout << setw(4*(indentation+1)) << "" << "return 0;" << endl;
    cout << setw(4*indentation) << "}" << endl;
}




