/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : modbus.ino
   Version du fichier : 1.0
   Auteur  : D. KIHN - Institut Limayrac
   Date    : janv 2015 . 2017 . 2019
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
        Ce module comprend les fonctions principales
------------------------------------------*/

#include "modbus.h"
#include <Wire.h>

/*-------------- Variables globales du programme ----*/
word memoire[TAILLE_MEMOIRE+1];	// Les registres MODBUS W0001 à W0030 (mots de 16 bits)
bool userUpd[TAILLE_MEMOIRE+1]={false}; // signale une mise à jour depuis le maître
byte Request[MAXREQUESTLEN +1]; // La requete du maitre ou la reponse de l'esclave (le tableau d'octets)
word RequestLen=0;	        // La requete du maitre ou la reponse de l'esclave (sa taille)
char SerialBuffer[MAXSERIALBUFFERLEN+1]; // le tampon de la liaison serie (le tableau d'octets)
word SerialBufferLen=0;                  // le tampon de la liaison serie (sa taille)
boolean SerialReceved=false;             // le tampon de la liaison serie est vide (pour l'instant)


/*-------  Fonction d'intialisation ---------*/
void setup(){
    for( int i=0; i<=TAILLE_MEMOIRE ; i++) memoire[i]=0; memoire[3]=0xABCD; // initialisation de la memoire
    Serial.begin(9600); // ATTENTION choisissez NL & CR dans le moniteur
    Wire.begin();
    pinMode(13, OUTPUT); // Sortie sur la LED
    pinMode(A0, INPUT);  // entree sur le CAN 0
    delay(2000);
    initDS1307();
    Serial << MPROMPT;
}

/*------  La boucle principale -------------*/
void loop(){        
    if( Serial.available() > 0 ){ SerialReadLine(); return; }
    if( SerialReceved ){ TraiterRequete(); return; }
    if( faire_un_cycle() ){
        //---- acquisition cyclique des valeurs toutes les TEMPS_DE_CYCLE_AUTOMATE millisecondes
        PORTB ^= 0x20;     // pour "voir" la LED changer a chaque cycle
        getInternalTemp(); // mesurer temperature interne
        setVirtualPort();  // gestion du port de sortie virtuel (bandeau LED entre GND et 8
        getVirtualPort();  // gestion du port d'entrée broches 3 à 7
        WriteDS1307();     // gestion de l'horloge
        ReadDS1307();      // gestion de l'horloge
        //---- AJOUTEZ ICI VOS PROPRES SCRUTATIONS --------------
        getEtatDuCapteur();// votre propre acquisition de quelque chose
    }
}//END loop()

boolean TraiterRequete( void ){
     //---- Decodage trame ASCII -> RTU
     Serial << F("Requete du maitre    : ") << SerialBuffer << "\n"; 
     SerialBufferVersRequete(); 
     LibererSerialBuffer();
     //----------- cette requete est-elle pour nous ? ---------------------------------
     if( Request[0] != ESCLAVE ) return exception( "Requete adressee a un autre esclave.");
     
     //----------- traitement de la requete du maitre ---------------------------------
     switch( Request[1] ){
     case 0x03: //fonction lecture
          if( !ModBusFonctionLire() ) return false;
          break;
     case 0x06: //fonction ecriture
          if( !ModBusFonctionEcrire1word() ) return false;
          break;
     case 0x41: //fonction 65 identification
          if( !ModBusFonctionIdentification() ) return false;
          break;
     default: // fonction non reconnue
          RepondreErreur( 0x01 ); // illegal function
          break;
     } 
     //---- Encodage trame RTU -> ASCII
     RequeteVersSerialBuffer(); 
     Serial << F("Reponse de l'esclave : ") << SerialBuffer << '\n';
     Serial << MPROMPT; // afficher le prompt
     LibererSerialBuffer();
}




