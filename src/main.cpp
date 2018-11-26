#include <LibRobus.h>
#include <MFRC522.h>
#include <SPI.h>
#include <LiquidCrystal.h>

//Variable contenant les commandes des clients
String commandeActuelle = "";

//Variables pour le deplacement
float vTarget = 0.20; // vitesse initial du robot
float vMax = vTarget + 0.02;
float vMin = vTarget - 0.02;
int positionRobot = 0; // position de depart du robot
float ACCEL = 0.01; //acceleration du robot
bool isScanning = true; 

//Variables des capteurs
int gauche = 0;
int milieu = 0;
int droite = 0;

//Define des capteurs
#define GAUCHEAVANT 35  //capteur de gauche en avant
#define MILIEUAVANT 33  //capteur du milieu en avant
#define DROITEAVANT 37  //capteur de droite en avant
#define GAUCHEARRIERE 41    //capteur de gauche en arriere
#define MILIEUARRIERE 43    //capteur du milieu en arriere
#define DROITEARRIERE 39    //capteur de droite en arriere

//TODO: changer les pins
//Define des LED
#define LEDC1 45    //LED du contenant1
#define LEDC2 47    //LED du contenant2 
#define LEDC3 49    //LED du contenant3

//Define des pompes
#define POMPE_BLEU 44    //Pompe bleu
#define POMPE_ROUGE 45    //Pompe rouge
#define POMPE_JAUNE 46   //Pompe jaune
#define PWM 100     //pwm sur les pompes

//Define de l'ecran LCD et RFID
#define RS 13
#define EN 11
#define D4 7
#define D5 4
#define D6 3
#define D7 2

#define RST_PIN 9
#define SS_PIN  53


//Define angle du depart
#define INITIAL 180   //angle initial
#define SERVIR  0     //angle pour la position des pompes

//Variables pour l'ecran LCD et RFID
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
MFRC522 rfid(SS_PIN, RST_PIN);
bool paiementNonEffectuee = true;

byte nuidPICC[4];

//Structure de la commande
struct Commande
{
    int position = 0;
    int couleur = 0;
};

//Structure des contenants
struct Contenants
{
    float quantiteC1 = 0;
    float quantiteC2 = 0;
    float quantiteC3 = 0;
};

//Variables des contenants
struct Contenants contenantUtilise;

const int couleurs [6][2] = {
    {POMPE_BLEU, POMPE_BLEU},
    {POMPE_ROUGE, POMPE_ROUGE},
    {POMPE_JAUNE, POMPE_JAUNE},
    {POMPE_ROUGE, POMPE_JAUNE},
    {POMPE_BLEU, POMPE_JAUNE},
    {POMPE_BLEU, POMPE_ROUGE}
};

//Fonctions prototypes
void read_Bluetooth();
bool VerificationContenants();
void UpdateContenants(const int couleur);
struct Commande SeparationCommande(String commandeActuelle);
void Deplacement(const int positionClient);
void EffectuerCommande(struct Commande commande);
void FaireDrink(const int couleur);
void resetContenants();
bool paiementRFID();

void setup() 
{
    BoardInit();
    Serial.begin(9600);
    BluetoothInit();
    BLUETOOTH_setCallback(read_Bluetooth);
    pinMode(GAUCHEAVANT, INPUT); //gauche
    pinMode(MILIEUAVANT, INPUT); //milieu
    pinMode(DROITEAVANT, INPUT); //droite
    pinMode(GAUCHEARRIERE, INPUT); //gauche-arriere
    pinMode(MILIEUARRIERE, INPUT); //milieu-arriere
    pinMode(DROITEARRIERE, INPUT); //droite-arriere
    // pinMode(LEDC1, OUTPUT); //LED contenant1
    // pinMode(LEDC2, OUTPUT); //LED contenant2
    // pinMode(LEDC3, OUTPUT); //LED contenant3
    pinMode(POMPE_BLEU, OUTPUT);    //pompe contenant1
    pinMode(POMPE_ROUGE, OUTPUT);    //pompe contenant2  
    pinMode(POMPE_JAUNE, OUTPUT);    //pompe contenant3
    // lcd.begin(16, 2);
    // SPI.begin();
    // rfid.PCD_Init();
    SERVO_Enable(SERVO_1); //servomoteur
    SERVO_SetAngle(SERVO_1, INITIAL);
    delay(1000);
    SERVO_Disable(SERVO_1);
}


void loop()
{
    if(ROBUS_IsBumper(FRONT)) {
        resetContenants();
    }

    if (!VerificationContenants())
    {
        Deplacement(0);
        return;
    }

    if (commandeActuelle.length() < 3)
    {
        delay(200);
        return;
    }

    struct Commande commande = SeparationCommande(commandeActuelle);
    Deplacement(commande.position);

    // while (paiementNonEffectuee)
    // {
    //     paiementNonEffectuee = paiementRFID();
    // }
    // EffectuerCommande(commande);
}

void read_Bluetooth() //assigne l'information reçu de l'application
{
    commandeActuelle = "";
    while(commandeActuelle.length() < 3) 
    {
        commandeActuelle += BLUETOOTH_read();
        //Serial.println(commandeActuelle);
    }
}

bool VerificationContenants() //LED d'un contenant s'allume s'il reste juste assez de liquide pour une consommation (quand il reste 30 ml)
{
    if (contenantUtilise.quantiteC1 > 3) 
    {
        digitalWrite(LEDC1, HIGH);
        return false;
    }
    if (contenantUtilise.quantiteC2 > 3)
    {
        digitalWrite(LEDC2, HIGH);
        return false;
    }
    if (contenantUtilise.quantiteC3 > 3)
    {
        digitalWrite(LEDC3, HIGH);
        return false;
    }

    digitalWrite(LEDC1, LOW);
    digitalWrite(LEDC2, LOW);
    digitalWrite(LEDC3, LOW);
    return true;
}

void resetContenants() {
    contenantUtilise.quantiteC1 = 0;
    contenantUtilise.quantiteC2 = 0;
    contenantUtilise.quantiteC3 = 0;
}

void UpdateContenants(const int couleur)
{
    switch(couleur)
    {
        case 1:
            contenantUtilise.quantiteC1++;
            break;
        case 2:
            contenantUtilise.quantiteC2++;
            break;
        case 3:
            contenantUtilise.quantiteC3++;
            break;
        case 4:
            contenantUtilise.quantiteC2+= 0.5;
            contenantUtilise.quantiteC3+= 0.5;
            break;
        case 5:
            contenantUtilise.quantiteC1+= 0.5;
            contenantUtilise.quantiteC3+= 0.5;
            break;
        case 6:
            contenantUtilise.quantiteC1+= 0.5;
            contenantUtilise.quantiteC2+= 0.5;
            break;
        default: break;
    }
}

struct Commande SeparationCommande(String commande) //Separe la position et la couleur de la String de la commande
{
    struct Commande commandeDesire;
    int index = commande.indexOf(',');

    commandeDesire.position = commande.substring(0,index).toInt();
    commandeDesire.couleur = commande.substring(index + 1).toInt();

    return commandeDesire;
}

void Deplacement(int positionClient) //Deplacement du robot
{
    // Pars de sa position de depart jusqu'à la position du client à l'aide du suiveur de lignes.
    int NbrLignes = positionClient - positionRobot;
    float speed = vTarget;
    int sens;
    int motor;

    if (NbrLignes == 0)
    {
        commandeActuelle = "";
        Serial.println("Position deja atteinte");
        return; //le robot est deja au bon endroit, servir le drink;
    }

    if (NbrLignes > 0)
    {
        gauche = GAUCHEAVANT;
        milieu = MILIEUAVANT;
        droite = DROITEAVANT;
        motor = RIGHT;
        sens = 1;
    }
    else
    {
        gauche = GAUCHEARRIERE;
        milieu = MILIEUARRIERE;
        droite = DROITEARRIERE;
        motor = LEFT;
        sens = -1;
    }

    MOTOR_SetSpeed(RIGHT, (sens * vTarget));
    MOTOR_SetSpeed(LEFT, (sens * vTarget));

    Serial.println(positionClient);
    Serial.println(positionRobot);
    Serial.println();
    while(positionRobot != positionClient) { 
        delay(100);

        if (!digitalRead(GAUCHEAVANT) && !digitalRead(MILIEUAVANT) && !digitalRead(DROITEAVANT)) {
            if (!isScanning) {
                isScanning = true;
                positionRobot += sens;
                Serial.println(positionRobot);
            }
        }
        else {
            isScanning = false;
            if(!digitalRead(gauche)) {
                if(speed < vMax) {
                    speed += ACCEL;
                }
            }                
            else if(!digitalRead(droite)) {
                if (speed > vMin) {
                    speed -= ACCEL;
                    Serial.println(speed);
                }
            }
            else if (!digitalRead(milieu)) {
                speed = vTarget;
            }
            MOTOR_SetSpeed(motor, (speed*sens));
            MOTOR_SetSpeed(!motor, (vTarget*sens));
        }
    }
    Serial.println("Reussis");
    commandeActuelle = "";
    MOTOR_SetSpeed(LEFT, 0);
    MOTOR_SetSpeed(RIGHT, 0);
}

bool paiementRFID() //Retourne vrai si le paiment n'est pas effectuee et faux s'il est fait
{
    lcd.setCursor(0, 0);
    lcd.print("Paiement : 2,50$");
    if ( ! rfid.PICC_IsNewCardPresent()) 
    {
        return true;
    }
    if ( ! rfid.PICC_ReadCardSerial()) 
    {
        return true;
    }
    if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] ||
        rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] ) 
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Transaction...");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Paiment Recu!");
        delay(1000);
        return false;
    }
    return true;
}

void EffectuerCommande(struct Commande commande)
{
    UpdateContenants(commande.couleur);

    SERVO_Enable(SERVO_1);
    SERVO_SetAngle(SERVO_1, SERVIR);
    delay(1000);
    SERVO_Disable(SERVO_1);

    FaireDrink(commande.couleur);
    
    SERVO_Enable(SERVO_1);
    SERVO_SetAngle(SERVO_1, INITIAL);
    delay(1000);
    SERVO_Disable(SERVO_1);
}

void FaireDrink(const int couleur) //Bouge le servomoteur et active les pompes pour faire le drink
{
    const int* pompes = couleurs[couleur];
    uint8_t pompe;
    for(int i = 0; i < 2; i++) {

        pompe = uint8_t(pompes + i);

        analogWrite(pompe,PWM);
        delay(700);
        analogWrite(pompe, 0);
        delay(1000);
    }
}
