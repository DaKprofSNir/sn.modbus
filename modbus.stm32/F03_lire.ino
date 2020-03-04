/*------------------------------------------
    Projet  : Automate MODBUS version 2.0
    Fichier : F03_LIRE.ino                   | Version du fichier : 1.0
    Fonction du module : ModBus Fonction 3 = Lire n mots
    Auteur  : D. KIHN - Institut Limayrac    | Date    : oct 2019
    Format de la requête :                   Format de la réponse :
    | byte N°Esclave                         | byte N°Esclave
    | byte 0x03                              | byte 0x03
    | word N° du 1ier registre à lire        | byte Nombre d'octets qui suivent
    | word Nombre de registres à lire        | word,[word,...] autant de valeurs que demandées
    | word crc16                             | word crc16
    -------------------------------------------------*/
boolean ModBusFonctionLire( void ){
    // N°Escl+N°Fonc+@+Nbr+crc = 1+1+2+2+2 = 8 octets
    if( !Verifier_len_crc(8) ) return false;
    //--- verifier les parametres ------------------
    word Adresse = Request[2]*256 + Request[3];
    if( Adresse>TAILLE_MEMOIRE) return RepondreErreur( 0x02 ); //illegal data address
    word NbMots  = Request[4]*256 + Request[5];
    if( Adresse+NbMots > TAILLE_MEMOIRE+1) return RepondreErreur( 0x03 ); //illegal data value
    //---- construire la reponse ------------------
    // Request[0] = ESCLAVE; // inchangé
    // Request[1] = 0x03;    // inchangé
    Request[2] = NbMots * 2 ; // on renvoit 2 octets pour chaque mot de 16 bits
    if( NbMots==0 ){
        RequestLen = 3; // rien à ajouter si on ne lit que zéro mots
    }else{
        for( byte i=0, j=3; i<NbMots ; i++, j+=2 ){
            Request[ j   ] = (byte)(memoire[Adresse+i] >> 8);
            Request[ j+1 ] = (byte)(memoire[Adresse+i] & 0x00FF);
            RequestLen = j+2; // la taille de la réponse a pris 2 octets de plus
        }
    }
    //----------- ajout du CRC à la trame reponse ---------------------------------
    AjouteCRC();
    return true;
}//ModBusFonctionLire
