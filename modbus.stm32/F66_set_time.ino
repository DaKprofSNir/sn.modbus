/*------------------------------------------
   Projet  : Automate MODBUS version 2.1
   Fichier : F66.ino
   Version du fichier : 1.0
   Auteur  : D. KIHN - Institut Limayrac
   Date    : octobre 2015
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
   ModBus Fonction 17 = Mise a l'heure UTC
   Execute puis construit la trame de r�ponse � la fonction 0x42
   
   Format de la requ�te :
      byte NoEsclave
      byte 0x42
      word UTC MSB
      word UTC LSB
      word crc16
   Format de la r�ponse :
      byte NoEsclave
      byte 0x42
      word UTC MSB
      word UTC LSB
      word crc16
-------------------------------------------------*/
boolean ModBusSetTime( void ){
     word crc;
     unsigned long SETUTC;
     //---- verifier la taille de la requete -------
     // Escl+Fonc+MSB+LSB+crc = 1+1+2+2+2 = 8 octets
     if( RequestLen<8 ) return exception( F("Erreur de transmission (octets manquants).") );
     if( RequestLen>8 ) return exception( F("Erreur de transmission (octets surnumeraires).") );    
     //---- verifier le crc sur les 6 premiers octets
     crc = CalculCrc16( Request,RequestLen-2 );
     if( (Request[RequestLen-2] != (crc & 0x00FF) ) ||
         (Request[RequestLen-1] != (crc >> 8)     ) )
         return exception( F("Erreur de transmission (crc).") );
     //--- Lire les parametres ------------------
     SETUTC = (unsigned long)Request[2]<<24 | (unsigned long)Request[3]<<16 |(unsigned long)Request[4]<<8 | (unsigned long)Request[5];
     setTime(SETUTC); 
     //---- construire la reponse ------------------
     Request[0] = ESCLAVE;
     Request[1] = 0x42;
     Request[2] = (byte)(SETUTC >> 24);
     Request[3] = (byte)(SETUTC >> 16);
     Request[4] = (byte)(SETUTC >> 8);
     Request[5] = (byte)(SETUTC >> 0);
     RequestLen = 6; // la taille de la reponse
     //----------- ajout du CRC � la trame reponse ---------------------------------
     crc = CalculCrc16( Request, RequestLen);
     Request[RequestLen++] = crc & 0x00FF;
     Request[RequestLen++] = crc >> 8;
     return true;
}
