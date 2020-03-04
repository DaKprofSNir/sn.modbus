/*------------------------------------------
   Projet  : Automate MODBUS version 2.0
   Fichier : iCAPTEUR.ino
   Version du fichier : 1.0
   Auteur  : <inscrivez ici votre nom>
   Date    : janv 2015
   REF     : 1SN - S7 Reseaux locaux industriels
                   S5 Solutions constructives

   Fonction du module :
     Lecture de la valeur renvoyee par un capteur <de température GROOVE>
     Le resultat est place dans W0004
     Le résultat est exprimé en centièmes de degrés celcius
------------------------------------------*/

void getEtatDuCapteurGrooveTMP( void ){
    float temperature, resistance;
    int a, B=3975; //B value of the thermistor
    delay(10);
    a=analogRead(0);
    delay(1);
    resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
    temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet
    memoire[ 0004 ] = 100 * temperature;
    // la valeur deviendra automatiquement accessible par la fonction 3 MODBUS
}
    
