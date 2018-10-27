#include <LibRobus.h>

void ResetEncodeurs();
int CalculerErreurDroite();
void Avancer();
void Avancer(const int TEMPS);
void Reculer();
void Reculer(const int TEMPS);
void Tourner(const int TEMPS);
void Tourner(const int MS, const int MOTEUR);

bool Son();
void Random();
void Pattern();
// void Avancer(int pulse);
// void DemiTour();
enum choix {
    AVANCER = 1,
    RECULER = 2,
    GAUCHE = 3,
    DROITE = 4
};


//poser une vitesse x pour moteur 0 et 1
//0.6
const float vTarget = 0.8;

//tier de vTarget
const float vDemiVitesse = vTarget / 3;
const float ACCEL = 0.2;
float vGauche = 0;
float vDroite = vGauche;

//Variables pour le PID
const float KP = 0.001; //IDEAL : 0.001
const float KI = 0.0001; //IDEAL : 0.0001
int erreurDroite = 0;
int erreurTotale = 0;
int tempsCycle = 150; //en ms

//Variables pour la fonction random 
//(ajustements pour un peu moins de random)
int lastRand = 1;
int nbA = 0;
int nbR = 0;

void setup() {
    BoardInit();
    //delay(2000);
    Serial.begin(9600);
    Pattern();
    //MOTOR_SetSpeed(LEFT, 0);
    //MOTOR_SetSpeed(RIGHT, 0);
}

void loop() {
    Random();
}

void Random() {
    int random = 0;
    int temps = 0;
    
    // do {
    //     random = (rand() % 2) + 1;
    //     temps = ((rand() % 2) + 1) * 1000 ;
    // } while(random == (lastRand < 3 ? lastRand: lastRand - 2) );

    // if(lastRand < 3) {
    //     random += 2;
    // }

    // if(random < 3){
    //     if(nbA > 2){
    //         Reculer(2500);
    //         lastRand = choix::AVANCER;
    //         nbA = 0;
    //         return;
    //     }
    //     if(nbR > 2) {
    //         Avancer(2500);
    //         lastRand = choix::RECULER;
    //         nbR = 0;
    //         return;
    //     }
    // }

    //trouver une valeur random à exécuter.
        //si le dernier choix était tourner, on va choisir avancer/reculer
        //si le dernier choix était avancer/reculer, on va choisir tourner
    do {
        random = (rand() % 2) + 1;
        temps = ((rand() % 2) + 1 )* 1000;

        if(lastRand < 3)
            random += 2;

    }while(random == lastRand);

    lastRand = random;

    switch(random) {
        //AVANCER
        case choix::AVANCER: 
            if(nbA > 2) {
                Reculer(temps);
                nbA = 0;
                return;
            }
            Avancer(temps);
            nbA++; 
            nbR = 0; 
            break;
        //RECULER
        case choix::RECULER: 
            if(nbR > 2) {
                Avancer(temps);
                nbR = 0;
                return;
            }
            Reculer(temps);
            nbR++; 
            nbA = 0; 
            break;
        //TOURNER GAUCHE
        case choix::GAUCHE: Tourner(temps/8, LEFT); break;
        //TOURNER DROITE
        case choix::DROITE: Tourner(temps/8, RIGHT); break;
        //shouldnt be an issue
        default: break;
    }

}

void Pattern() {
    Avancer(600);
    Tourner(tempsCycle, LEFT);
    Avancer(600);
    Tourner(tempsCycle, RIGHT);
    Avancer(2500);
    Tourner(tempsCycle * 2, LEFT);
    Avancer(1500);
}

void Tourner(const int MS, const int MOTEUR) {
    int vitesse = vGauche < 0 ? vTarget: -vTarget;
    
    MOTOR_SetSpeed(MOTEUR, vitesse);

    for(int i = 0; i < MS; i += tempsCycle) {
        if(Son()) {
            MOTOR_SetSpeed(LEFT, 0);
            MOTOR_SetSpeed(RIGHT, 0);
            delay(10000);
            MOTOR_SetSpeed(MOTEUR, vitesse);
            MOTOR_SetSpeed(!MOTEUR, -vitesse);
        } else {
            delay(100);
        }
    }

    MOTOR_SetSpeed(MOTEUR,-vitesse);

    delay(tempsCycle);
    ResetEncodeurs();
}

void Avancer(){

    ResetEncodeurs();

    if(vGauche < vTarget) {
        vGauche += ACCEL;
        vDroite += ACCEL;
    }
    //SET VITESSE
    MOTOR_SetSpeed(LEFT, vGauche);
    MOTOR_SetSpeed(RIGHT, vDroite);
    
    //ATTEND UN CYCLE
    delay(100);
    if(Son()) {
        MOTOR_SetSpeed(RIGHT, 0);
        MOTOR_SetSpeed(LEFT, 0);
        delay(10000);
        vGauche = 0;
        vDroite = 0;
    }
    //CALCULE L'ERREUR DE DROITE VS GAUCHE 
    erreurDroite = CalculerErreurDroite();
    erreurTotale += erreurDroite;

    //CHANGE LA VITESSE PAR RAPPORT À L'ERREUR
    vDroite += (erreurDroite * KP) + (erreurTotale * KI) ;

}

void Avancer(const int TEMPS) {
    for(int tps = 0;tps < TEMPS; tps += tempsCycle) {
        Avancer();
    }
} 

void Reculer(){

    ResetEncodeurs();

    if(vGauche > -vTarget) {
        vGauche -= ACCEL;
        vDroite -= ACCEL;
    }
    //SET VITESSE
    MOTOR_SetSpeed(LEFT, vGauche);
    MOTOR_SetSpeed(RIGHT, vDroite);
    
    //ATTEND UN CYCLE
    delay(100);
    if(Son()) {
        MOTOR_SetSpeed(RIGHT, 0);
        MOTOR_SetSpeed(LEFT, 0);
        delay(10000);
        vGauche = 0;
        vDroite = 0;
    }
    //CALCULE L'ERREUR DE DROITE VS GAUCHE 
    erreurDroite = CalculerErreurDroite();
    erreurTotale += erreurDroite;

    //CHANGE LA VITESSE PAR RAPPORT À L'ERREUR
    vDroite += (erreurDroite * KP) + (erreurTotale * KI) ;

    if(Son()) {
        MOTOR_SetSpeed(RIGHT, 0);
        MOTOR_SetSpeed(LEFT, 0);
        delay(10000);
        vGauche = 0;
        vDroite = 0;
    }
}

void Reculer(const int TEMPS) {
    for(int tps = 0; tps < TEMPS; tps += tempsCycle) {
        Reculer();
    }
}

void ResetEncodeurs(){
    ENCODER_Reset(LEFT);
    ENCODER_Reset(RIGHT);
}

int CalculerErreurDroite() {
    return ENCODER_Read(LEFT) - ENCODER_Read(RIGHT); 
}

bool Son() {
    delay(50);
    return false;
    //Serial.println(analogRead(A0) * (5.0/1023.0) );
    for(int i =0; i< 4; i++){
        bool isHigh = ( analogRead(A0) * (5/1023.0) ) > 2.8;
        if(!isHigh) {
            return false;
        }

        delay(50);
    }
    return true;
}