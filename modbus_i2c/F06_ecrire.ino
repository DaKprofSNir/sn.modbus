/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : modbus_F03_LIRE.ino
   Version du fichier : 1.0
   Auteur  : D. KIHN - Institut Limayrac
   Date    : sept 2017
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
   ModBus Fonction 3 = Lire n mots
	
    Construit la trame de réponse à la fonction 0x03
    Format de la requête :
      byte N°Esclave
      byte 0x06
      word N° du registre à écrire
      word Valeur à écrire
      word crc16
    Format de la réponse :
      byte N°Esclave
      byte 0x03
      byte Nombre d'octets qui suivent
      word,[word,...] autant de valeurs que demandées
      word crc16
-------------------------------------------------*/
boolean ModBusFonctionEcrire1word( void ){
     word Adresse, Data, crc;
     //---- verifier la taille de la requete -------
     // N°Escl+N°Fonc+@+data+crc = 1+1+2+2+2 = 8 octets
     if( RequestLen<8 ) return exception( "Erreur de transmission (octets manquants)." );
     if( RequestLen>8 ) return exception( "Erreur de transmission (octets surnumeraires)." );    
     //---- verifier le crc sur les 6 premiers octets
     crc = CalculCrc16( Request,RequestLen-2 );
     if( (Request[RequestLen-2] != (crc & 0x00FF) ) ||
         (Request[RequestLen-1] != (crc >> 8)     ) )
         return exception( "Erreur de transmission (crc).");
     //--- verifier les parametres ------------------
     Adresse = Request[2]*256 + Request[3];
     if( Adresse>TAILLE_MEMOIRE) return RepondreErreur( 0x02 ); //illegal data address
     Data  = Request[4]*256 + Request[5];
     if( Adresse > TAILLE_MEMOIRE+1) return RepondreErreur( 0x03 ); //illegal data value
	 //---- Faire le job ---------------------------
	 memoire[Adresse] = Data;
     userUpd[Adresse] = true;
     //---- construire la reponse ------------------
	 //La réponse est identique à la question
     //Request[0] = ESCLAVE;
     //Request[1] = 0x06;
	 //etc
     return true;
}




