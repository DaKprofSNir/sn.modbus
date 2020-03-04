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
 
#ifdef ARDUINO_ARCH_AVR
float getInternalTemp(void){
  float t; word can,lsb,msb;
  ADMUX = 0xC8;                // Sélectionne le port analogique 8 + référence interne 1v1
  delay(10);                    // Attente de la stabilisation de la tension de référence
  ADCSRA |= (1 << ADEN);       // Active le convertisseur analogique -> numérique
  ADCSRA |= (1 << ADSC);       // Lance une conversion analogique -> numérique
  while(ADCSRA & (1 << ADSC)); // Attend la fin de la conversion
  can = ADCL | (ADCH << 8);    // Récupère le résultat de la conversion
  t = (can - 320.31 ) / 1.22;  // Calcule la température (can - 324.31 ) / 1.22;
  //Serial << "getInternalTemp " << can << " " << t << '\n';
  DecomposerFloat( t, &msb, &lsb );
  memoire[ 0001 ] = msb;
  memoire[ 0002 ] = lsb;
  return t;
}
#endif

#ifdef ARDUINO_ARCH_SAMD
float getInternalTemp(void){
  memoire[ 0001 ] = 26;
  memoire[ 0002 ] = 31;
  return 26.31;
}
#endif

#ifdef ARDUINO_ARCH_STM32
// http://electroniqueamateur.blogspot.com/2017/09/mesurer-une-temperature-avec-une-carte.html
float getInternalTemp(void){
  int a=analogRead(PADC_TEMP);
  float v=a*(3.3/4096.0);
  float t=(1.42-t)/0.043-10.0;
#ifdef DEBUG
    static float oldval=-150.0;
    if(t != oldval){
        oldval = t;
        Serial.print("#DEBUG getInternalTemp ");
        Serial.println(t);
    }
#endif  
  
  memoire[ 0001 ] = t;
  memoire[ 0002 ] = (t*100-memoire[1]*100);
  return t;
}
#endif  
 
void DecomposerFloat( float valeur, word* partie_entiere, word* partie_decimale ){
    if( valeur<0 ) valeur = 0; // 
    *partie_entiere = valeur;
    valeur = valeur - *partie_entiere;
    *partie_decimale = (valeur*100);  
}
   
