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
    for (i=0; i<taille_trame_utile; i++){
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
}


word HexToBin( char hexa ){
   return ( hexa>='A' )? (10 + hexa - 'A') : (hexa - '0') ;
}

char MsbToHex( byte b ){ b&=15; return (b<10)?('0'+b):('A'+b-10); }
char LsbToHex( byte b ){ b>>=4; return (b<10)?('0'+b):('A'+b-10); }
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
}

void RequeteVersSerialBuffer(void){
   word i,j;   byte msb,lsb;
   for( i=0,j=0; j<RequestLen; j++, i+=2 ){
       msb=Request[j]>>4; lsb=Request[j]&0x0F;
       SerialBuffer[i+0] = (msb<=9) ? ('0'+msb) : ('A'+msb-10);
       SerialBuffer[i+1] = (lsb<=9) ? ('0'+lsb) : ('A'+lsb-10);       
   }
   SerialBuffer[i]=0;
   SerialBufferLen = i;
}

void LibererSerialBuffer(void){
     SerialBufferLen=0; SerialBuffer[0] = 0; SerialReceved=false;
}


int SerialReadLine(void){
  if( SerialReceved )return -1 ;
  char incomingByte;
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
                    while( Serial.available() > 0 ){ Serial.read(); }
                    LibererSerialBuffer();
                    return -1;          
            }else{
                SerialReceved = true;
                return SerialBufferLen;
            }
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
                    while( Serial.available() > 0 ){ Serial.read(); }
                    LibererSerialBuffer();
                    return -1;
                }
            }
        }
    }
    return -1;
}


boolean faire_un_cycle_Automate(void){
    static uint32_t prochaine_mesure = 0;
    if( millis() >= prochaine_mesure ){
        prochaine_mesure = millis() + TEMPS_DE_CYCLE_AUTOMATE;
        return true;
    }else{
        return false;
    }
}

//boolean exception( const char* message ){
boolean exception( const __FlashStringHelper *message ){
  Serial << F("                       ") << message << CRLF;
  Serial << F("Reponse de l'esclave : (pas de reponse)") << CRLF;
  Serial << MPROMPT; // afficher le prompt
  LibererSerialBuffer();
  return false; // pas de reponse
}

void PrintDeviceUUID(){ 
    // Attention spécifique Atmega328 ! Si autre µP utiliser https://github.com/ricaun/ArduinoUniqueID
    Serial << F("DeviceUUID : ");
    for( size_t i=0x000E; i<=0x0017; i++ ){
        byte n = boot_signature_byte_get(i);
        Serial << MsbToHex(n) << LsbToHex(n) << (((i&3)==3)?" ":"") ; 
    }
    Serial << CRLF; 
}


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
}
  
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
}

void AjouteCRC(void){
     //----------- ajout du CRC à la trame reponse ---------------------------------
     word crc = CalculCrc16( Request, RequestLen);
     Request[RequestLen++] = crc & 0x00FF;
     Request[RequestLen++] = crc >> 8;
}     
