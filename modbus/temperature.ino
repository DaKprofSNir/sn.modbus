/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : temperature.ino
   Version du fichier : 1.0
   Auteur  : D. KIHN - Institut Limayrac
   Date    : oct 2019
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
     Lecture de la temperature interne de l'arduino
     Hey oui il y a un capteur de température dans l'ATmega32 !
     Le resultat est place dans W0001 et W0002
------------------------------------------*/
 
float getInternalTempOrg(void){
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

float getInternalTemp(void){
  float t; word can,lsb,msb; uint32_t r;
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
  //--------- test
  r=  (can*100-32431)/122 ;
  Serial << "t=" << t << CRLF << "r=" << r << CRLF ;  
  return t;
}

word ToBDC(word data){
   word BCDvalue=0x0000;
   if( data>9999 ) return 0x9999;
   for(byte i=0;i<8;i++){
        if( (BCDvalue&0x000F)>=5 )  BCDvalue+=0x0003;
        if( ((BCDvalue>>4)&0x000F)>=5 ) BCDvalue+=0x0030;
        if( ((BCDvalue>>8)&0x000F)>=5 ) BCDvalue+=0x0300;
        BCDvalue=BCDvalue<<1;
        if (0x80&data) BCDvalue++;
        data=data<<1;
    }
    return BCDvalue;
}   

uint16_t double_dabble (uint16_t Fbin){ 
  uint16_t Fbcd, W=0x8000;
    while(W){
        Fbcd +=(((Fbcd+0x3333)&0x8888)>>3)*3; 
        Fbcd<<=1 ;
        if(Fbin&W)Fbcd++;
        W >>=1 ; 
    }
    return Fbcd;
}


uint16_t ToBCD160 (uint16_t input ){
    uint16_t output=0;
     for(byte a = 13; a >= 0; a--){
        if((output & 0xF) >= 5)            output += 3;
        if(((output & 0xF0) >> 4) >= 5)    output += (3 << 4);
        if(((output & 0xF00) >> 8) >= 5)   output += (3 << 8);
        output = (output << 1) | ((input >> a) & 1);
    }
    return output;
}
