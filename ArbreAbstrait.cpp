#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include "Interpreteur.h"
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    for (Noeud* n : m_instructions){
        n->traduitEnCPP(cout,indentation);
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation) << ((SymboleValue*) m_variable)->getChaine();
    cout << "=";
    //m_expression->traduitEnCPP(cout, 0);  
    ((SymboleValue*) m_expression)->traduitEnCPP(cout,indentation);
    
    cout << ";" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    m_operandeGauche->traduitEnCPP(cout,indentation);
    cout << m_operateur.getChaine();
    m_operandeDroit->traduitEnCPP(cout,indentation);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSi::traduitEnCPP (std::ostream& cout, unsigned int indentation) const{
    cout << setw(4*indentation) << " " << "if (";
    m_condition->traduitEnCPP(cout, 0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+1);
    cout << setw(4*indentation) << " " << "}" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
  while (m_condition->executer()) {
      m_sequence->executer();
  }
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation) << " " << "while (";
    m_condition->traduitEnCPP(cout, 0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+1);
    cout << setw(4*indentation) << " " << "}" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* sequence , Noeud* condition)
:m_sequence(sequence),m_condition(condition) {
}


int NoeudInstRepeter::executer(){

    do{
        m_sequence->executer() ;
    } while (!m_condition->executer()) ; 
    return 0 ; 
}

void NoeudInstRepeter::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation) << " " << "do {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+1);
    cout << setw(4*indentation) << " " << "} while (";
    m_condition->traduitEnCPP(cout, 0);
    cout << ");" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* initialisation, Noeud* expression, Noeud* iteration, Noeud* sequence): m_initialisation(initialisation),m_condition(expression),m_iteration(iteration),m_sequence(sequence){}

int NoeudInstPour::executer(){
    for(m_initialisation->executer();m_condition->executer();m_iteration->executer()){
    
        m_sequence->executer();
    }
    
}

void NoeudInstPour::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << setw(4*indentation) << " " << "for (";
    m_initialisation->traduitEnCPP(cout, 0);
    //if (m_initialisation!=nullptr) cout << "; ";
    m_condition->traduitEnCPP(cout, 0);
    if (m_iteration!=nullptr) cout << "; ";
    m_iteration->traduitEnCPP(cout,0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+1);
    cout << setw(4*indentation) << " " << "}" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire():m_expressions(){
}
void NoeudInstEcrire::ajouter(Noeud* n ){
    m_expressions.push_back(n) ; 

}

int NoeudInstEcrire::executer(){
    for(Noeud* p : m_expressions){
        if (typeid(*p)==typeid(SymboleValue) && *((SymboleValue*)p)== "<CHAINE>"){
            string chaine = ((SymboleValue*)p)->getChaine();
            cout << chaine.substr(1,chaine.length()-2);
        }
        else {
            cout << p->executer(); 
        }
    }
    return 0 ; 
}

void NoeudInstEcrire::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << "cout";
    for (Noeud* p : m_expressions){
        if (typeid(*p)==typeid(SymboleValue) && *((SymboleValue*)p)== "<CHAINE>"){
            cout << " << " << ((SymboleValue*)p)->getChaine();
        }
        else {
            cout << " << ";
            p->traduitEnCPP(cout,0);
        }
    }
    cout << ";" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(): m_variables() {}

void NoeudInstLire::ajouter(Noeud* n){
    m_variables.push_back(n);
}

int NoeudInstLire::executer(){
    for(Noeud* p : m_variables){
        //string valS;
        int valI;
        //if (*((SymboleValue*)p) == "<CHAINE>"){
            cin >> valI;
            ((SymboleValue*)p)->setValeur(valI);
        //}
        //else {
        //    cin >> valS;
        //    ((SymboleValue*)p)->setValeur(valS);
        //}
    }
    return 0 ; 
}

void NoeudInstLire::traduitEnCPP(std::ostream& cout, unsigned int indentation) const {
    cout << "cin";
    for (Noeud* p : m_variables){
        cout << " >> ";
        p->traduitEnCPP(cout,0);
    }
    cout << ";" << endl;
}

