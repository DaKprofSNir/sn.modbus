/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : modbus_TEMPERATURE.ino
   Version du fichier : 1.0
   Auteur  : D. KIHN - Institut Limayrac
   Date    : janv 2015
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
     Lecture de la temperature interne de l'arduino
     Hey oui il y a un capteur de température dans l'ATmega32 !
     Le resultat est place dans W0001 et W0002
------------------------------------------*/
 
float getInternalTemp(void){
  float t; word can,lsb,msb;
  ADMUX = 0xC8;                // Sélectionne le port analogique 8 + référence interne 1v1
  ADCSRA |= (1 << ADEN);       // Active le convertisseur analogique -> numérique
  delay(1);                    // Attente de la stabilisation de la tension de référence
  ADCSRA |= (1 << ADSC);       // Lance une conversion analogique -> numérique
  while(ADCSRA & (1 << ADSC)); // Attend la fin de la conversion
  can = ADCL | (ADCH << 8);    // Récupère le résultat de la conversion
  t = (can - 324.31 ) / 1.22;  // Calcule la température
  DecomposerFloat( t, &msb, &lsb );
  memoire[ 0001 ] = msb;
  memoire[ 0002 ] = lsb;
  return t;
}
 
void DecomposerFloat( float valeur, word* partie_entiere, word* partie_decimale ){
    if( valeur<0 ) valeur = 0; // 
    *partie_entiere = valeur;
    valeur = valeur - *partie_entiere;
    *partie_decimale = (valeur*100);  
}
   
