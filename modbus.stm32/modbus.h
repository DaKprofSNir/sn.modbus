/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : modbus.h
   Version du fichier : 1.0
   Auteur  : D. KIHN - Institut Limayrac
   Date    : janv 2015 mars 2019
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
      Declarations communes
------------------------------------------*/

#include <Arduino.h>

//---- definitions de l'esclave modbus
#define ESCLAVE 0x04
#define TAILLE_MEMOIRE 30
#define TEMPS_DE_CYCLE_AUTOMATE 2000
extern word memoire[TAILLE_MEMOIRE+1]; // les W1 à W30. Attention les mots commencent à 1 en MODBUS

/*--- definition des fonctions implémentées -----
03 LIRE un mot de 16 bits dans les registres internes W0001 à W0029
65 Chaine d'identification
       definition des registres internes
W0001  Temperature interne partie entiere
W0002  Temperature interne partie decimale
W0004  capteur GROOVE utilisé dans le TP
W0005  Port virtuel de sortie, les bits 0 à 5 correspondent au bandeau de LEDs branché entre GND et 8
W0006  Port virtuel d'entrée,  les bits 0 à 5 correspondent aux broches 3 à 7
W0010  Premier chantillonnage entree analogique A0 - voir sujet de tp
W0021..W0024 libre
W0025 UTC poid fort
W0026 UTC poid faible
W0029  Echantillonnage n°30 entree analogique A0
--------------------------------------------*/
//---- Identification           0123456789ABCDEF
#define CHAINEIDENTIFICATION F("ModbusAVR v2.0")
//---- Fonctionnalités supportées
#define USE_LCD
#define USE_RTCinterne
#define USE_DS1307
#define USE_GROVE_NTC


template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } 
#define CRLF F("\n")
#define MPROMPT F(":)\n")

//--- Le tableau d'octets qui contient la trame recue ou la trame a emettre
#define MAXREQUESTLEN 25
extern byte Request[MAXREQUESTLEN +1];
extern word RequestLen;

//---- Le tampon d'emission et de reception de la liaison serie
#define MAXSERIALBUFFERLEN (MAXREQUESTLEN*2+1)
extern char SerialBuffer[MAXSERIALBUFFERLEN+1];
extern word SerialBufferLen;
extern boolean SerialReceved;
