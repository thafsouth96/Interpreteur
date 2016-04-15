#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
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

NoeudInstRepeter::NoeudInstRepeter(Noeud* sequence , Noeud* condition)
:m_sequence(sequence),m_condition(condition) {
}


int NoeudInstRepeter::executer(){

    do{
        m_sequence->executer() ;
    } while (!m_condition->executer()) ; 
    return 0 ; 
}


NoeudInstPour::NoeudInstPour(Noeud* initialisation, Noeud* expression, Noeud* iteration, Noeud* sequence): m_initialisation(initialisation),m_condition(expression),m_iteration(iteration),m_sequence(sequence){}

int NoeudInstPour::executer(){
    for(m_initialisation->executer();m_condition->executer();m_iteration->executer()){
    
        m_sequence->executer();
    }
    
}


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

NoeudInstLire::NoeudInstLire(): m_variables() {}

void NoeudInstLire::ajouter(Noeud* n){
    m_variables.push_back(n)
}

int NoeudInstLire::executer(){
    for(Noeud* p : m_variables){
        if (((SymboleValue*)p) == "<CHAINE>")){
            string val;
        }
        else 
        cin >> ;
    }
    return 0 ; 
}

