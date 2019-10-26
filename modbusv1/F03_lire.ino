/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : modbus_F03_LIRE.ino
   Version du fichier : 1.0
   Auteur  : D. KIHN - Institut Limayrac
   Date    : janv 2015
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
   ModBus Fonction 3 = Lire n mots
	
    Construit la trame de réponse à la fonction 0x03
    Format de la requête :
      byte N°Esclave
      byte 0x03
      word N° du 1ier registre à lire
      word Nombre de registres à lire
      word crc16
    Format de la réponse :
      byte N°Esclave
      byte 0x03
      byte Nombre d'octets qui suivent
      word,[word,...] autant de valeurs que demandées
      word crc16
-------------------------------------------------*/
boolean ModBusFonctionLire( void ){
     word Adresse, NbMots, crc;
     //---- verifier la taille de la requete -------
     // N°Escl+N°Fonc+@+Nbr+crc = 1+1+2+2+2 = 8 octets
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
     NbMots  = Request[4]*256 + Request[5];
     if( Adresse+NbMots > TAILLE_MEMOIRE+1) return RepondreErreur( 0x03 ); //illegal data value
     //---- construire la reponse ------------------
     Request[0] = ESCLAVE;
     Request[1] = 0x03;
     Request[2] = NbMots * 2 ; // on renvoit 2 octets pour chaque mot de 16 bits
     if( NbMots==0 ){
          RequestLen = 3; // rien à ajouter si on ne lit que zéro mots
     }else{
          for( int i=0, j=3; i<NbMots ; i++, j+=2 ){
              Request[ j   ] = (byte)(memoire[Adresse+i] >> 8);
              Request[ j+1 ] = (byte)(memoire[Adresse+i] & 0x00FF);
              RequestLen = j+2; // la taille de la réponse a pris 2 octets de plus
          }
     }
     //----------- ajout du CRC à la trame reponse ---------------------------------
     crc = CalculCrc16( Request, RequestLen);
     Request[RequestLen++] = crc & 0x00FF;
     Request[RequestLen++] = crc >> 8;
     return true;
}

