#define DEBUG1307
#undef  DEBUG1307

//registres DS1307   La date par defaut est lundi 25 mars 2019 9h50
//                   sec min heure l..d jour mois annee
byte ds1307reg[] = {0x00,0x50,0x09,0x01,0x25,0x03,0x19};

/* Ces registres seront recopies ensuite vers la memoire d'echange :
W0011  date AAMM
W0012  date JJhh
W0013  date mmss
*/
void initDS1307(){
    //----- lecture du bit CH pour savoir si le DS1307 a besoin d'être initialisé
    byte testCHbit=0;
    Wire.beginTransmission(0x68);              // Start
    Wire.write(0);                             // addresse du premier registre
    Wire.endTransmission();                    // Stop
	  Wire.requestFrom(0x68,sizeof(testCHbit));  // Start+lecture+stop 
    testCHbit=Wire.read();                     // contient le bit CH
    if(testCHbit>=0x80U){  // le bit 7 du registre zero est CH
        // ecriture de la date par defaut qui est dans le tampon ds1307reg
        Wire.beginTransmission(0x68); // Start
        Wire.write(0);                // addresse du registre zero
        Wire.write(ds1307reg, sizeof(ds1307reg) );
        Wire.endTransmission();       // Stop
    }//if(testCHbit)
}//END initDS1307()

void WriteDS1307(){
    // test si le maitre a demande une ecriture
    boolean pasDeMaj=true;
    for(byte i=11;i<=14;i++) if(userUpd[i]) pasDeMaj=userUpd[i]=false;
    if(pasDeMaj) return;
    // recopie vers les registres de l'horloge
    ds1307reg[0]=memoire[13]&0x00FF; // secondes
    ds1307reg[1]=memoire[13]>>8;     // minutes
    ds1307reg[2]=memoire[12]&0x00FF; // heure
    ds1307reg[4]=memoire[12]>>8;     // jour
    ds1307reg[5]=memoire[11]&0x00FF; // mois
    ds1307reg[6]=memoire[11]>>8;     // annee
    Wire.beginTransmission(0x68);              // Start
    Wire.write(0);                             // addresse du premier registre
    Wire.write(ds1307reg, sizeof(ds1307reg) ); // ecriture vers le composant i2c
    Wire.endTransmission();                    // Stop
}//END WriteDS1307()

void ReadDS1307(){
    //----- lecture des registres du DS1307 RTC --------------
    Wire.beginTransmission(0x68);              // Start
    Wire.write(0);                             // addresse du premier registre
    Wire.endTransmission();                    // Stop
	  Wire.requestFrom(0x68,sizeof(ds1307reg));  // Start+lecture+stop 
    //----- recuperer le contenu du tampon ------------------- 
    for(byte i=0;i<sizeof(ds1307reg);i++) ds1307reg[i]=Wire.read(); 
    //----- Recopie dans W0011..W0014 --------
    memoire[11] = (uint16_t)ds1307reg[5] | (uint16_t)ds1307reg[6]<<8; //AAMM
    memoire[12] = (uint16_t)ds1307reg[2] | (uint16_t)ds1307reg[4]<<8; //JJhh
    memoire[13] = (uint16_t)ds1307reg[0] | (uint16_t)ds1307reg[1]<<8; //mmss
#ifdef DEBUG1307
    Serial.print("DS1703 W11..W13 = 0x");
    for(byte m=11;m<=13;m++)for(byte q=3;q<4;q--)Serial.print(BinToHex(memoire[m],q));
    Serial.print('\n');
#endif
}//END ReadDS1307()


