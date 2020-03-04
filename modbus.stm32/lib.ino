/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : modbus_LIB.ino
   Version du fichier : 1.0
   Auteur  : D. KIHN - Institut Limayrac
   Date    : oct 2019
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

  Fonctions apportees par ce fichier :
  
word CalculCrc16( byte* Request, word taille_trame_utile )  Calcule le CRC16
word HexToBin( char hexa )         Conversion HEXA->Binaire
char MsbToHex( byte b )            Conversion Binaire->HEXA
char LsbToHex( byte b )            Conversion Binaire->HEXA
void PrintDeviceUUID()             imprime le numero de serie du microcontroleur
void SerialBufferVersRequete()     ASCII -> RTU
void RequeteVersSerialBuffer()     RTU -> ASCII
void LibererSerialBuffer(void)     Vider le tampon
int SerialReadLine()               Lire depuis la liaison serie
boolean exception( char* message ) avertir erreur lecture liaison serie
boolean faire_un_cycle()           true s'il est temps de faire un nouveau cycle automate
Fonction de chaine de caractères sRAZ et sADD
----------------------------------------------------------------*/



/*-------------------------------------------
   le crc16 detecte :
   100% des erreurs sur 1 à 16 bits
   99.9969% des erreurs s'il y a exactement 17 bits errones
   99.9984% des erreurs s'il y a plus que 17 bits errones
   sachant qu'une trame ne dépasse jamais 255 octets.
-------------------------------------------*/
word CalculCrc16( byte* Request, word taille_trame_utile ){
    word crc,i,n,retenue;
    crc=0xFFFF;
    for( i=0; i<taille_trame_utile; i++){
	      crc = crc ^ (unsigned char) Request[i];  
	      n = 0;
	      do{
	          retenue = crc & 1;
            crc = crc >> 1;
            if( retenue!=0 ) crc = crc ^ 0xA001;
            n++;
        }while (n <= 7);
    }
    return crc; 
}//CalculCrc16


word HexToBin( char hexa ){ return ( hexa>='A')?(10+hexa-'A'):(hexa-'0'); }
char LsbToHex( byte b ){ b&=15; return (b<10)?('0'+b):('A'+b-10); }
char MsbToHex( byte b ){ b>>=4; return (b<10)?('0'+b):('A'+b-10); }
/*-------------------------------------------------------
    Fonctions ASCII -> RTU  SerialBufferVersRequete
           et RTU -> ASCII  RequeteVersSerialBuffer()
--------------------------------------------------------*/
void SerialBufferVersRequete(void){
  word i,j;
  for( i=0,j=0; i<SerialBufferLen; i+=2,j++ ){
       Request[j] = HexToBin(SerialBuffer[i])*16 + HexToBin(SerialBuffer[i+1]);     
  }
  RequestLen = j;
}//SerialBufferVersRequete

void RequeteVersSerialBuffer(void){
   word i,j;   byte msb,lsb;
   for( i=0,j=0; j<RequestLen; j++, i+=2 ){
       msb=Request[j]>>4; lsb=Request[j]&0x0F;
       SerialBuffer[i+0] = (msb<=9) ? ('0'+msb) : ('A'+msb-10);
       SerialBuffer[i+1] = (lsb<=9) ? ('0'+lsb) : ('A'+lsb-10);       
   }
   SerialBuffer[i]=0;
   SerialBufferLen = i;
}//RequeteVersSerialBuffer

void LibererSerialBuffer(void){ 
    while( Serial.available()>0 )Serial.read();
    SerialBufferLen=0; 
    SerialBuffer[0] = 0; 
    SerialReceved=false; 
 }

int SerialReadLine(void){
    char incomingByte;
    //if( SerialReceved )return -1 ;
    if( Serial.available() > 0 ){
        incomingByte = Serial.read();
        switch( incomingByte ){
        case '\n': // Ignore new-lines
            break;
        case '\r': // Return on CR
            if( (SerialBufferLen&0x0001)==0x0001 ){
                Serial << F("Requete du maitre    : ") << SerialBuffer << incomingByte << CRLF;
                Serial << F("                       nombre impair de caracteres hexadecimaux ") << SerialBufferLen << CRLF;
                Serial << F( "Reponse de l'esclave : (pas de reponse)" ) << CRLF;
                Serial << MPROMPT; // afficher le prompt
                //--- libération du buffer de réception ----
                LibererSerialBuffer();
                //SerialReceved = true; <-pas la bonne place à cause de tolosair.ino ligne 53
            }else{
                SerialReceved = true;
            } break;
        default:
            if (SerialBufferLen < MAXSERIALBUFFERLEN) {
                if( ((incomingByte>='a') && (incomingByte<='f')) ){ incomingByte -=32; }
                if( ((incomingByte>='A') && (incomingByte<='F')) || 
                    ((incomingByte>='0') && (incomingByte<='9')) ){          
                    SerialBuffer[SerialBufferLen++] = incomingByte; 
                    SerialBuffer[SerialBufferLen] = 0;
                }else{
                    Serial << F("Requete du maitre    : ") << SerialBuffer << incomingByte << CRLF;
                    Serial << F("                       caractere non hexadecimal en position ") << (1+SerialBufferLen) << CRLF;
                    Serial << F("Reponse de l'esclave : (pas de reponse)") << CRLF;
                    Serial << MPROMPT; // afficher le prompt
                    //--- libération du buffer de réception ----
                    LibererSerialBuffer();
                }
            }else{
                LibererSerialBuffer();
            }//SerialBufferLen < MAXSERIALBUFFERLEN
        }
    }//if( Serial.available()
    return SerialBufferLen;
}//SerialReadLine


boolean faire_un_cycle_Automate(void){
    static uint32_t prochaine_mesure = 0;
    if( millis() >= prochaine_mesure ){
        prochaine_mesure = millis() + TEMPS_DE_CYCLE_AUTOMATE;
        return true;
    }else{
        return false;
    }
}//faire_un_cycle_Automate

//boolean exception( const char* message ){
boolean exception( const __FlashStringHelper *message ){
  Serial << F("                       ") << message << CRLF;
  Serial << F("Reponse de l'esclave : (pas de reponse)") << CRLF;
  Serial << MPROMPT; // afficher le prompt
  LibererSerialBuffer();
  return false; // pas de reponse
}//exception

void PrintDeviceUUID(){ 
  Serial << F("DeviceUUID : ");
  for (size_t i = 0; i < UniqueIDsize; i++){
        byte n = UniqueID[i];
        Serial << MsbToHex(n) << LsbToHex(n) << (((i&3)==3)?" ":"") ; 
    }
    Serial << CRLF; 
}//PrintDeviceUUID


boolean Verifier_len_crc(byte LEN){
    //---- verifier la taille de la requete -------
     if( RequestLen<LEN ) return exception( F("Erreur de transmission (octets manquants).") );
     if( RequestLen>LEN ) return exception( F("Erreur de transmission (octets surnumeraires).") );
     //---- verifier le crc
     word crc = CalculCrc16( Request,RequestLen-2 );
     if( (Request[RequestLen-2] != (crc & 0x00FF) ) ||
         (Request[RequestLen-1] != (crc >> 8)     ) )
         return exception( F("Erreur de transmission (crc).") );
    return true;
}//Verifier_len_crc
  
boolean RepondreErreur( byte NumErreur ){
     word crc;
     Request[0] = ESCLAVE;
     Request[1] |= 0x80; // bit 7 du code fonction à 1
     Request[2] = NumErreur;
     RequestLen = 3;
     crc = CalculCrc16( Request, RequestLen);
     Request[RequestLen++] = crc & 0x00FF;
     Request[RequestLen++] = crc >> 8;
     return true;
}//RepondreErreur

void AjouteCRC(void){
     //----------- ajout du CRC à la trame reponse ---------------------------------
     word crc = CalculCrc16( Request, RequestLen);
     Request[RequestLen++] = crc & 0x00FF;
     Request[RequestLen++] = crc >> 8;
}//AjouteCRC


// Fonction de chaine de caractères sRAZ et sADD
#ifndef _STRLENMAX
#define _STRLENMAX (128)
#endif
//extern char _str[_STRLENMAX];
//extern byte _strlen=0;
inline void sRAZ(void){ _strlen=0; _str[0]=0; }
inline void sRAZ(char *src){ _strlen=0; _str[0]=0; sADD( src ); }
inline void sRAZ(const __FlashStringHelper *src){ _strlen=0; _str[0]=0; sADD( src ); }
inline void sADD( char *src ){ byte i=0; while( (_strlen<_STRLENMAX) && (src[i]!=0) ) _str[_strlen++]= src[i++]; }
inline void sADD( char c ){ _str[_strlen]=c; _strlen++; _str[_strlen]=0; }
inline void sADD( int value  ){ itoa( value, _str+_strlen, 10 ); _strlen=strlen(_str); }
inline void sADD( long value ){ ltoa( value, (_str+_strlen), 10 ); _strlen=strlen(_str); }
inline void sADD( long msb, long lsb ){ sADD(  msb ); sADD( '.' ); sADD(lsb); }
inline void sADD(const __FlashStringHelper *fstr){ strcpy_P((_str+_strlen), (char *)fstr ); _strlen = strlen(_str); } 
inline void sADDx( byte b ){ 
  sADD( MsbToHex(b) );
  sADD( LsbToHex(b) );
}
inline void sADDn( word value, byte nbChiffres ){ sADDn(value,nbChiffres,10); }
inline void sADDn( word value, byte nbChiffres, byte base){
    char buffer[16]; byte i, len=0;
    itoa( value, buffer, base ); 
    len=strlen(buffer);
    if( len>nbChiffres ){ 
        for(byte i=0;i<=nbChiffres;i++) sADD('?');
    }else{
      for( byte i=0; i<nbChiffres-len; i++) sADD('0');
      sADD( buffer );
    }
}


void SurveilleFreeMemory(void){
  static uint16_t freeMem=0xFFFF;
  static uint16_t errCount=0xFFFF;
  uint16_t temp;
  if( (temp=freeMemory()) < freeMem ){
      freeMem = temp;
      errCount++;
      Serial << CRLF << F("! freeMemory()=") << freeMemory() << F(" ErrCount=") << errCount << CRLF;
  }
}//SurveilleFreeMemory

//---------------------------------------
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


void ToggleLED(){
   // AVR
   // PORTB ^= 0x20;
   static int LEDstate=-1;
   if(LEDstate<0){ pinMode(LED_BUILTIN, OUTPUT); LEDstate=LOW; }
   digitalWrite(LED_BUILTIN, LEDstate=(LEDstate==LOW)?HIGH:LOW );
}//ToggleLED()
