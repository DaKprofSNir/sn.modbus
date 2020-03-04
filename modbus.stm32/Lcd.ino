#ifdef USE_LCD

char lcd_line0[] = "0123456789ABCDEF0123456789ABCDEF0";
//char lcd_line1[] = "0123456789ABCDEF0123456789ABCDEF0";
long currentStep = 0;

void lcd_init(){
    lcd.begin(16, 2);
    lcd.print(CHAINEIDENTIFICATION);
    lcd.setCursor(0, 1);
    lcd.print("Init");    
}
void lcd_step(){
    const unsigned long TEMPS_DE_CYCLE_LCD = 100; //100ms
    static unsigned long next = 0;
    static unsigned int stepUpperLine = 0;
    static unsigned int stepLowerLine = 0;
   if( millis() >= next ){
        next = millis() + TEMPS_DE_CYCLE_LCD;
        stepUpperLine++;
        stepLowerLine++;
    }else{return;}
    //--------- UTC ----------
    if((stepLowerLine%20)==0){
        //0123456789ABCDEF
        // 2015   17:30:07
        //15/12   17:30:07
        lcd.setCursor(0, 1);
        time_t t = now();
        sRAZ();
        sADD(' '); sADDn( year(t),4 ); sADD( F("   ") );
        sADDn( hour(t),2); sADD(':'); sADDn( minute(t),2); sADD(':');sADDn( second(t),2);   
        lcd.print(_str);
        //sprintf(lcd_line1," %04u   %02u:%02u:%02u", year(t), hour(t), minute(t), second(t) ); 
        //lcd.print(lcd_line1);
    }else if((stepLowerLine%20)==10){
        lcd.setCursor(0, 1);
        time_t t = now();
        sRAZ();
        sADDn( month(t),2 ); sADD('/'); sADDn(day(t),2); sADD( F("   ") );
        sADDn(  hour(t),2); sADD(':'); sADDn( minute(t),2); sADD(':');sADDn( second(t),2);   
        lcd.print(_str);
        //sprintf(lcd_line1,"%02u/%02u   %02u:%02u:%02u", month(t), day(t), hour(t), minute(t), second(t) );       
        //lcd.print(lcd_line1);
    }
    //----------ANIM----------
    if(stepUpperLine<100){
      lcd.setRGB(128,128,128);
      strcpy_P(lcd_line0, (char*)CHAINEIDENTIFICATION);
      strcat(lcd_line0,"                ");
      lcd.setCursor(0, 0);
      lcd.print(lcd_line0);
    }else if(stepUpperLine<116){
      lcd_line0[stepUpperLine-100]=0x20;
      lcd.setCursor(0, 0);
      lcd.print(lcd_line0);
    }else if(stepUpperLine<167){
      // 0123456789ABCDEF
      // T int: 33.33 Deg
      lcd.setRGB(0,200,63);
      lcd.setCursor(0, 0);
      lcd.print(F("T int: "));
      lcd.print(memoire[0001]);
      lcd.print('.');
      lcd.print(memoire[0002]);
      lcd.print(F(" Deg  "));
    }else{ stepUpperLine=0;}
}


#endif
