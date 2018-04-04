#define F_CPU 4000000
//#define F_CPU 4000000UL
//unsigned char Sec,Min,Hour,Day,Month,Year,DayOfWeek,AMPM,H12_24;
//unsigned char menu,mode,buff[16],alarmNum,alarmNumB,ch,bgl;
#include "main.h"
//***************************************************************************
// Установки

//***************************************************************************
// Инициализация
void avr_init(){
	
}
//***************************************************************************

//***************************************************************************
int main (void){
	unsigned char bm,bs,i;
	sei();
	avr_init();
	while(1){
		
		delay(1000);
	}
}