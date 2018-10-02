#include <LibRobus.h>

void ResetEncodeurs();
int CalculerErreurDroite();

//poser une vitesse x pour moteur 0 et 1
float vTarget = 1;
float vGauche = 0.15;
float vDroite = vGauche;

const float KP = 0.001; //IDEAL : 0.001
const float KI = 0.01; //IDEAL : 0.0001

//float ki = 0;

int erreurDroite = 0;
int erreurTotale = 0;
int tempsCycle = 200; //en ms

void setup() {

    BoardInit();
    //allumer les deux moteurs
    MOTOR_SetSpeed(LEFT, vGauche);
    MOTOR_SetSpeed(RIGHT, vDroite);
}

void loop() {
    //REMET LES ENCODEURS À 0
    ResetEncodeurs();

    //SI TROP LENT, AUGMENTER VITESSE
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
    erreurTotale += erreurDroite * tempsCycle / 1000;

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
