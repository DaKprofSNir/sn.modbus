/*------------------------------------------
    Projet  : Automate MODBUS version 2.0
    Fichier : F06_ECRIRE.ino                 | Version du fichier : 1.0
    Fonction du module : ModBus Fonction 6 ecrire 1 mot
    Auteur  : D. KIHN - Institut Limayrac    | Date    : oct 2019
    Format de la requête :                   Format de la réponse :
    | byte N°Esclave                         | byte N°Esclave
    | byte 0x06                              | byte 0x06
    | word N° du registre à écrire           | word N° du registre à écrire
    | word Valeur à écrire                   | word Valeur ecrite
    | word crc16                             | word crc16
-------------------------------------------------*/
boolean ModBusFonctionEcrire1word( void ){
    //---- verifier la taille de la requete -------
    // N°Escl+N°Fonc+@+data+crc = 1+1+2+2+2 = 8 octets
    if( !Verifier_len_crc(8) ) return false;
    //--- verifier les parametres ------------------
    word Adresse = Request[2]*256 + Request[3];
    if( Adresse>TAILLE_MEMOIRE) return RepondreErreur( 0x02 ); //illegal data address
    word Data  = Request[4]*256 + Request[5];
if( Adresse>TAILLE_MEMOIRE+1) return RepondreErreur( 0x03 ); //illegal data value
    //---- Faire le job ---------------------------
    memoire[Adresse] = Data;
    //---- construire la reponse ------------------
    //ici la réponse est identique à la question
    //Request[0] = ESCLAVE;
    //Request[1] = 0x06;
    //etc
    return true;
}//ModBusFonctionEcrire1word
