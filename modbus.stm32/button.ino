
#define PIN_BUTTON 5
boolean Scruter_boutton(void){
    // scruter � intervalle de 1 � 10 ms
    static byte states=0;
    static boolean value=false;
    states<<1;
    if( digitalRead(PIN_BUTTON)==HIGH ) states |= 0x01;
    if( states==0xFF ) value=true; else if( states==0x00 ) value=false;
    return value;
}
