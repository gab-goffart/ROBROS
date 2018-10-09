#include <LibRobus.h>

const int LIGNE1 = 26805;
const int LIGNE2 = 2680;
const int LIGNE4 = 600;
const int LIGNE5 = 3400;
const int LIGNE6 = 4800;
const int LIGNE7 = 2680;
const int LIGNE8 = 9381;

const int QUART = 650;

void ResetEncodeurs();
int CalculerErreurDroite();
void TournerDroite(int ms);
void TournerGauche(int ms);
void Avancer();



//poser une vitesse x pour moteur 0 et 1
//0.7
float vTarget = 0.7;
//0.2
float vDemiVitesse = 0.2;
float vGauche = 0.15;
float vDroite = vGauche;

const float KP = 0.001; //IDEAL : 0.001
const float KI = 0.0001; //IDEAL : 0.0001

//float ki = 0;

int erreurDroite = 0;
int erreurTotale = 0;
int tempsCycle = 150; //en ms
int dParcouru = 0;

void setup() {

    BoardInit();
    //allumer les deux moteurs
    MOTOR_SetSpeed(LEFT, vGauche);
    MOTOR_SetSpeed(RIGHT, vDroite);
}

void loop() {

    while(dParcouru < LIGNE1){
        Avancer();
        dParcouru += ENCODER_Read(LEFT);
    }
    dParcouru = 0;
    
    TournerGauche(QUART);

    TournerDroite(QUART);

    Avancer();

    TournerDroite(QUART);   

    while(dParcouru < LIGNE4) {
        Avancer();
        dParcouru += ENCODER_Read(LEFT);
    }
    dParcouru = 0;
    
    
    TournerGauche(QUART);   
    TournerDroite(2*QUART/3);


    while(dParcouru < LIGNE5) {
        Avancer();
        dParcouru += ENCODER_Read(LEFT);
    }
    dParcouru = 0;

    
    TournerGauche(QUART);   

    while(dParcouru < LIGNE6) {
        Avancer();
        dParcouru += ENCODER_Read(LEFT);
    }
    dParcouru = 0;

    TournerDroite(QUART / 2);

    while(dParcouru < LIGNE7) {
        Avancer();
        dParcouru += ENCODER_Read(LEFT);
    }
    dParcouru = 0;

    TournerDroite(QUART / 4);

    while(dParcouru < LIGNE8) {
        Avancer();

        dParcouru += ENCODER_Read(LEFT);
    }
    dParcouru = 0;

    vGauche = 0;
    vDroite = 0;

    MOTOR_SetSpeed(LEFT, vGauche);
    MOTOR_SetSpeed(RIGHT, vDroite);
    
    dParcouru = 0;

    exit(0);

}

void TournerGauche(int ms) {
    MOTOR_SetSpeed(LEFT, vDemiVitesse);
    delay(ms);
    MOTOR_SetSpeed(LEFT, vGauche);
    delay(100);
    ResetEncodeurs();
}

void TournerDroite(int ms) {
    MOTOR_SetSpeed(RIGHT, vDemiVitesse);
    delay(ms);
    MOTOR_SetSpeed(RIGHT, vDroite);
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

void ResetEncodeurs(){
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
}

int CalculerErreurDroite() {
    return ENCODER_Read(LEFT) - ENCODER_Read(RIGHT); 
}
