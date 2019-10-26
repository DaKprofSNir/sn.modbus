/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : modbus_TEMPERATURE.ino
   Version du fichier : 1.0
   Auteur  : D. KIHN - Institut Limayrac
   Date    : sept 2017
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
     Renvoyer la chaine d'identification de l'automate

   L'algorithme du traitement de cete fonction est semblable à modbus_F03_LIRE
   Pour que son appel soit possible Vous devez aussi décommenter l'appel qui 
   se trouve dans le programme principal modbus.ino fonction TraiterRequete()

------------------------------------------*/
 
boolean ModBusFonctionIdentification( void ){
     word crc;
     //---- verifier la taille de la requete -------
     // N°Escl+N°Fonc+crc = 1+1+2 = 4 octets
     if( RequestLen<4 ) return exception( "Erreur de transmission (octets manquants)." );
     if( RequestLen>4 ) return exception( "Erreur de transmission (octets surnumeraires)." );    
     
     //---- verifier le crc sur les 6 premiers octets
     crc = CalculCrc16( Request,RequestLen-2 );
     if( (Request[RequestLen-2] != (crc & 0x00FF) ) ||
         (Request[RequestLen-1] != (crc >> 8)     ) )
         return exception( "Erreur de transmission (crc).");
         
     //--- verifier les parametres ------------------
          // il n'y a pas de paramètres pour cette fonction
          
          
     //---- construire la reponse ------------------
     RequestLen = 0; // on reutilise le meme tampon de communication
     Request[0] = ESCLAVE;
     Request[1] = 0x41;
     Request[2] = 0 ; // le nombre de caracteres constituant la chaine d'identification
     RequestLen = 3; // 3 octets pour l'instant ! car bientot nous allons ajouter la chaine de caracteres.
     
     //---- copie de la chaine de caractere a renvoyer (a partir de Request[3])
     byte iden[] = CHAINEIDENTIFICATION;
     for( int i=0; iden[i]>0; i++){
        Request[ RequestLen ] = iden[i];
        RequestLen++;
     }
     Request[2] = RequestLen-3 ; // le nombre de caracteres constituant la chaine d'identification

     //----------- ajout du CRC à la trame reponse ---------------------------------
     crc = CalculCrc16( Request, RequestLen);
     Request[RequestLen++] = crc & 0x00FF;
     Request[RequestLen++] = crc >> 8;
     return true;
//   // et pour que finalement cela fonctionne, relisez le commentaire situe en tete ce ce fichier.
}
