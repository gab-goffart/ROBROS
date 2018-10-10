#include <LibRobus.h>

const int LIGNE1 = 26805;
const int LIGNE2 = 500;
const int LIGNE3 = 500;
const int LIGNE4 = 400;
const int LIGNE5 = 2400;
const int LIGNE6 = 6000;
const int LIGNE7 = 2700;
const int LIGNE8 = 12000;

const int QUART = 780;

void ResetEncodeurs();
int CalculerErreurDroite();
void Avancer();
void Avancer(int pulse);
void Tourner(const int MS, const int MOTEUR);
void DemiTour();
void Partie1();
void Partie2();



//poser une vitesse x pour moteur 0 et 1
//0.6
const float vTarget = 0.6;

//tier de vTarget
const float vDemiVitesse = vTarget / 3;
float vGauche = 0.15;
float vDroite = vGauche;

const float KP = 0.001; //IDEAL : 0.001
const float KI = 0.0001; //IDEAL : 0.0001

int erreurDroite = 0;
int erreurTotale = 0;
int tempsCycle = 150; //en ms
int dParcouru = 0;

// void setup() {


//     BoardInit();
//     MOTOR_SetSpeed(LEFT, vTarget);
//     MOTOR_SetSpeed(RIGHT, vTarget);
    
//     Avancer(8000);
//     Tourner(QUART, RIGHT);
//     MOTOR_SetSpeed(LEFT, 0);
//     MOTOR_SetSpeed(RIGHT, 0);
// }

void setup() {

    BoardInit();

    Partie1();

    DemiTour();

    Partie2();
}

void loop() {

    exit(0);

}

void Tourner(const int MS, const int MOTEUR) {
    
    MOTOR_SetSpeed(MOTEUR, vDemiVitesse);
    delay(MS);
    MOTOR_SetSpeed(MOTEUR, vTarget);
    delay(tempsCycle);
    ResetEncodeurs();
}

void Avancer(){

    ResetEncodeurs();

    if(vGauche < vTarget) {
        vGauche += 0.05;
        vDroite += 0.05;
    }
    //SET VITESSE
    MOTOR_SetSpeed(LEFT, vGauche);
    MOTOR_SetSpeed(RIGHT, vDroite);
    
    //ATTEND UN CYCLE
    delay(tempsCycle);

    //CALCULE L'ERREUR DE DROITE VS GAUCHE 
    erreurDroite = CalculerErreurDroite();
    erreurTotale += erreurDroite;

    //CHANGE LA VITESSE PAR RAPPORT À L'ERREUR
    vDroite += (erreurDroite * KP) + (erreurTotale * KI) ;
}

void Avancer(const int pulse) {
    while (dParcouru < pulse ) {
        Avancer();
        dParcouru += ENCODER_Read(LEFT);
    }

    dParcouru = 0;
}

void ResetEncodeurs(){
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
}

int CalculerErreurDroite() {
    return ENCODER_Read(LEFT) - ENCODER_Read(RIGHT); 
}

void DemiTour() {
    MOTOR_SetSpeed(LEFT, 0.5);
    MOTOR_SetSpeed(RIGHT, -0.5);

    delay(4.90 * tempsCycle);

    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);

    delay(1000);
    ResetEncodeurs();
}

void Partie1() {
    Avancer(LIGNE1);
    
    Tourner(QUART, LEFT);
    
    Avancer(LIGNE2);
    
    Tourner(QUART, RIGHT);

    Tourner(QUART, RIGHT);

    Avancer(LIGNE4);
    
    Tourner(QUART, LEFT);

    Avancer(LIGNE4);

    Tourner(QUART / 3, RIGHT);

    Avancer(LIGNE5);
    
    Tourner(QUART, LEFT);

    Avancer(LIGNE6);

    Tourner(QUART / 2, RIGHT);

    Avancer(LIGNE7);

    Tourner(QUART / 6, RIGHT);

    Avancer(LIGNE8);

    vGauche = 0;
    vDroite = 0;

    MOTOR_SetSpeed(LEFT, vGauche);
    MOTOR_SetSpeed(RIGHT, vDroite);
    
    dParcouru = 0;

    delay(1000);
}

void Partie2() {
    Avancer(LIGNE8);

    Tourner(QUART / 6, LEFT);

    Avancer(LIGNE7);

    Tourner(QUART / 2, LEFT);

    Avancer(LIGNE6 - 500);

    Tourner(QUART, RIGHT);

    Avancer(LIGNE5);

    Tourner(QUART / 3, LEFT);

    Avancer(LIGNE4);

    Tourner(QUART, RIGHT);

    Avancer(LIGNE4);

    Tourner(1.35 * QUART, LEFT);

    Tourner(1.35 * QUART, LEFT);

    Avancer(LIGNE2);

    Tourner(QUART, RIGHT);

    Avancer(LIGNE1);

    vGauche = 0;
    vDroite = 0;

    MOTOR_SetSpeed(LEFT, vGauche);
    MOTOR_SetSpeed(RIGHT, vDroite);
    
    dParcouru = 0;

    delay(tempsCycle * 5);
}