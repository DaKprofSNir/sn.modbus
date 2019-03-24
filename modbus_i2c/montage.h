/*    
-----------------------------------------------------------------------------------------------------------
              Entrées/sorties digitales

L'esclave MODBUS comporte deux ports virtuels de 16 bits.
Le port de sortie est accessible par le mot W0005. Les bits de poid faible sont mappés aux broches 8 à 13.
Le port d' entrée est accessible par le mot W0006. Les bits de poid faible sont mappés aux broches 2 à 7.

Vous pouvez connecter un barled entre 8 et GND pour voir les bits en sortie.
Vous pouvez connecter un dupont male-male entre GND et l'une des broches 2 à 7 pour entrer un bit.
-----------------------------------------------------------------------------------------------------------
              Composants I2C
Cet esclave MODBUS utilise le bus I2C. 

Une horloge DS1307 doit être connectee aux broche SDA SCL VCC et GND.
La date et l'heure seront exportés dans les mots W11 à W13, a lire avec la fonction 03.
La mise de la date et de l'heure sont possibles via plusieurs appels a la fonction 06.
-----------------------------------------------------------------------------------------------------------

*/
