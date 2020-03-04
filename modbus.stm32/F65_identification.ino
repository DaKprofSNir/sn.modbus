/*------------------------------------------
    Projet  : Automate MODBUS version 2.0
    Fichier : f65_identification.ino         | Version du fichier : 1.0
    Fonction du module : ModBus Fonction 65 = lire l'identification de l'automate
    Auteur  : D. KIHN - Institut Limayrac    | Date    : oct 2019
    Format de la requête :                   Format de la réponse :
    | byte N°Esclave                         | byte N°Esclave
    | byte 0x41                              | byte 0x41
    | word crc16                             | byte Nombre d'octets qui suivent
                                             | char,[char,...] autant de valeurs que necessaire
                                             | word crc16
------------------------------------------*/
 
boolean ModBusFonctionIdentification(void){
    //---- verifier la taille de la requete -------
    // N°Escl+N°Fonc+crc = 1+1+2 = 4 octets
    if( !Verifier_len_crc(4) ) return false;
    //--- verifier les parametres ------------------
      // il n'y a pas de paramètres pour cette fonction
    //---- construire la reponse ------------------
    RequestLen = 0; // on reutilise le meme tampon de communication
    Request[0] = ESCLAVE; // inchangé
    Request[1] = 0x41;    // inchangé
    Request[2] = 0 ; // le nombre de caracteres constituant la chaine d'identification
    RequestLen = 3; // 3 octets pour l'instant ! car bientot nous allons ajouter la chaine de caracteres.

    //---- recopie de la chaine de caractere a renvoyer (a partir de Request[3])
    //strcpy_P( Request+RequestLen, (char *)CHAINEIDENTIFICATION );
    strcpy_P( (char*)Request+RequestLen, (char*)CHAINEIDENTIFICATION );
    byte i= strlen( (char*)Request+RequestLen );
    RequestLen+=i;
    Request[2] = RequestLen-3 ; // le nombre de caracteres constituant la chaine d'identification
    AjouteCRC();
    return true;
}//ModBusFonctionIdentification
