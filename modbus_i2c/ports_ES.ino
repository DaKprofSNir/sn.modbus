/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : modbus_ES.ino
   Version du fichier : 1.0
   Auteur  : DaK
   Date    : mars 2019
   REF     : BTS SN - S7 Reseaux locaux industriels
                      S5 Solutions constructives

   Fonction du module :
     Gestion des ports virtuels
     Le port de sortie est W0005
     Le port d'entrée est W0006
------------------------------------------*/
#define ANIM1
//#undef  ANIM1

void setVirtualPort(){
  // W0005  Port virtuel de sortie, 
  // les bits 0 à 5 correspondent au bandeau de LEDs branché entre GND et 8
  static boolean init=true;
  static word fait = 0x00;
  word data = memoire[5];
  byte i;
  if( init ){
    for( i=8; i<=12; i++) pinMode(i,OUTPUT);
#ifdef ANIM1    
    for( i=8; i<=12; i++) {digitalWrite(i,HIGH); delay(1000);}
    for( i=8; i<=12; i++) {digitalWrite(i,LOW ); delay(1000);}
#else
    for( i=8; i<=12; i++) {digitalWrite(i,HIGH); }
    for( i=8; i<=12; i++) {digitalWrite(i,LOW ); delay(1000);}
    for( i=8; i<=12; i++) {digitalWrite(i,HIGH); delay(1000);}
    for( i=8; i<=12; i++) {digitalWrite(i,LOW); }
#endif    
    init=false;
  }
  if( fait==data ){return;}else{fait=data;}
  for( i=0; i<=5; i++) digitalWrite( i+8, ((data&(1<<i))==0)?LOW:HIGH );
}

void getVirtualPort(){
  // W0006  Port virtuel d'entrée, 
  // les bits 0 à 5 correspondent aux broches 2 à 7
  static boolean init=true;
  word data = memoire[6] & 0xFFC0; // 1111 1111 1100 0000
  byte i;
  if( init ){
    for( i=0; i<=5; i++) pinMode(i+2,INPUT_PULLUP);
    init=false;
  }
  for( i=0; i<=5; i++) if(digitalRead(i+2)==HIGH) data|=(1<<i);
  memoire[6]=data;
}
