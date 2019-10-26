#undef SAPERLIPOPETTE
#ifdef SAPERLIPOPETTE

-----------------------------------------------------------------------------------------------------------
              Entrées/sorties digitales

L'esclave MODBUS comporte deux ports virtuels de 16 bits.
Le port de sortie est accessible par le mot W0005. Les bits de poid faible sont mappés aux broches 8 à 13.
Le port d' entrée est accessible par le mot W0006. Les bits de poid faible sont mappés aux broches 2 à 7.

Vous pouvez connecter un barled entre 8 et GND pour voir les bits en sortie.
Vous pouvez connecter un dupont male-male entre GND et l'une des broches 2 à 7 pour entrer un bit.
-----------------------------------------------------------------------------------------------------------

#endif
