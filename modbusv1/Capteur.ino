/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : modbus_TEMPERATURE.ino
   Version du fichier : 1.0
   Auteur  : <inscrivez ici votre nom>
   Date    : janv 2015
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
     Lecture de la valeur renvoyee par un capteur <a completer>
     Le resultat est place dans W0004
------------------------------------------*/

void getEtatDuCapteur( void ){
     word ValeurCapteur = 0;
     

     //completez cette fonction en vous appuyant sur votre cours de physique
 
     
     memoire[ 0004 ] = ValeurCapteur;
     // la valeur deviendra automatiquement accessible par la fonction 3 MODBUS
}//END getEtatDuCapteur()
    
