#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define base  1193180 //внутренняя частота таймера
#define SHORT_MAX  65535

enum NOTE 
{
	c  = 261,
	d  = 294,
	e = 329,
	f = 349,
	g = 391,
	gS = 415,
	a = 440,
	aS = 455,
	b = 466,
	cH = 523,
	cSH = 554,
	dH = 587,
	dSH = 622,
	eH = 659,
	fH = 698,
	fSH = 740,
	gH = 784,
	gSH = 830,
	aH = 880
	
};

void printByte(char byte);

void printChanelStatus(char chanel) {
	char controlByte = 0xE0;		//11_1_0_000_0
	char chanelPort = 0x40;
	switch (chanel) {
	case 0: {
		controlByte = 0xE2;		//11_1_0_001_0 канал 0, бит 4 позволяет получить состояние при 0, бит 5 при 0 фиксирует счетчик, биты 6 и 7 в 1 - чтение 
		chanelPort = 0x40;
		break;
	}
	case 1: {
		controlByte = 0xE4;		//11_1_0_010_0 канал 1, бит 4 позволяет получить состояние при 0, бит 5 при 0 фиксирует счетчик, биты 6 и 7 в 1 - чтение 
		chanelPort = 0x41;
		break;
	}
	case 2: {
		controlByte = 0xE8;		//11_1_0_100_0 канал 2, бит 4 позволяет получить состояние при 0, бит 5 при 0 фиксирует счетчик, биты 6 и 7 в 1 - чтение 
		chanelPort = 0x42;
		break;
	}
	default: {
		printf("ERROR: Illegal printChanelStatus usage (ivalid chanel number %d)\n", chanel);
		return;
	}
	}
	outp(0x43, controlByte);
	char state = inp(chanelPort);
	printf("The \t%d-th chanel state: ", chanel);
	printByte(state);
	printf("\n");
}

void printByte(char byte) {
	char i;
	char byteBuf[8];
	for (i = 7; i >= 0; i--) {
		byteBuf[i] = (byte % 2) ? '1' : '0';
		byte /= 2;
	}
	for (i = 0; i < 8; i++) {
		printf("%c", byteBuf[i]);
	}
}

void printChanelKD(short chanel) {
	char controlByte;
	char chanelPort;
	switch (chanel) {
	case 0: {
		controlByte = 0xD0;		//11_0_1_001_0  канал 0,
		chanelPort = 0x40;
		break;
	}
	case 1: {
		controlByte = 0xD0;		//01_00_000_0 | 11_0_1_010_0 канал 1,
		chanelPort = 0x41; 
		break;
	}
	case 2: {
		controlByte = 0xD0;		//10_00_000_0	| 11_0_1_100_0 канал 2,
		chanelPort = 0x42;
		break;
	}
	default: {
		printf("ERROR: Illegal printChanelKD usage (ivalid chanel number %d)\n", chanel);
		return;
	}
	}
	unsigned short int kd = 0, kdHigh = 0, kdLow = 0, kdMax = 0, i;

	for (i = 0, kdMax = 0; i < SHORT_MAX; i++) {
		outp(0x43, controlByte);
		kdLow = inp(chanelPort);
		kdHigh = inp(chanelPort);
		kd = kdHigh * 256 + kdLow;
		if (kd > kdMax)
		{
			printf("new kdMax: %hx\n", kd);
			kdMax = kd;
		}
	}
	printf("The %d-th chanel kd: \t%hx\n", chanel, kdMax);
}


void random(long maxNumber) 
{
	long kdNew = 0, kdLow = 0, kdHigh = 0;
	long prevState = inp(0x61);
	outp(0x61, inp(0x61) & 0xFC | 0x01);	// disable dynamic, enable tick

	outp(0x43, 0xB4); // 10_11_010_0 генератор импульсов, чтение/запись младшего, а за ним старшего байта, выбор третьего канала 
	outp(0x42, maxNumber % 256);	// low byte
	outp(0x42, maxNumber / 256);	// high byte	

	int i = 0;
	for (i = 0; i < 5; i++)
	{
		delay(i);
		kdLow = inp(0x42);
		kdHigh = inp(0x42);
		kdNew = kdHigh * 256 + kdLow;
		printf("%d\n", kdNew);
	}
	outp(0x61, prevState);
}

void playSound(int note, int duration) {
	int kd;
	outp(0x43, 0xB6);//10_11_011_0 - выбор третьего канала, чтение/запись младшего, а за ним старшего байта, генератор прямоугольных импульсов

	kd = base / note;

	outp(0x42, kd % 256);
	kd /= 256;
	outp(0x42, kd);

	outp(0x61, inp(0x61) | 0x03);
	delay(duration);

	outp(0x61, inp(0x61) & 0xFC);
	// switch pause
	delay(duration / 3);
}

void doRock() 
{
	playSound( a, 500);
	playSound( a, 500);
	playSound( a, 500);
	playSound( f, 350);
	playSound( cH, 150);

	playSound( a, 500);
	playSound( f, 350);
	playSound( cH, 150);
	playSound( a, 1000);
	//first bit

	playSound( eH, 500);
	playSound( eH, 500);
	playSound( eH, 500);
	playSound( fH, 350);
	playSound( cH, 150);

	playSound( gS, 500);
	playSound( f, 350);
	playSound( cH, 150);
	playSound( a, 1000);
	//second bit...

	playSound( aH, 500);
	playSound( a, 350);
	playSound( a, 150);
	playSound( aH, 500);
	playSound( gSH, 250);
	playSound( gH, 250);

	playSound( fSH, 125);
	playSound( fH, 125);
	playSound( fSH, 250);
	delay(250);
	playSound( aS, 250);
	playSound( dSH, 500);
	playSound( dH, 250);
	playSound( cSH, 250);
	//start of the interesting bit

	playSound( cH, 125);
	playSound( b, 125);
	playSound( cH, 250);
	delay(250);
	playSound( f, 125);
	playSound( gS, 500);
	playSound( f, 375);
	playSound( a, 125);

	playSound( cH, 500);
	playSound( a, 375);
	playSound( cH, 125);
	playSound( eH, 1000);
	//more interesting stuff (this doesn't quite get it right somehow)

	playSound( aH, 500);
	playSound( a, 350);
	playSound( a, 150);
	playSound( aH, 500);
	playSound( gSH, 250);
	playSound( gH, 250);

	playSound( fSH, 125);
	playSound( fH, 125);
	playSound( fSH, 250);
	delay(250);
	playSound( aS, 250);
	playSound( dSH, 500);
	playSound( dH, 250);
	playSound( cSH, 250);
	//repeat... repeat

	playSound( cH, 125);
	playSound( b, 125);
	playSound( cH, 250);
	delay(250);
	playSound( f, 250);
	playSound( gS, 500);
	playSound( f, 375);
	playSound( cH, 125);

	playSound( a, 500);
	playSound( f, 375);
	playSound( c, 125);
	playSound( a, 1000);

}

int main() {
	// 1) get states
	printChanelStatus(0);
	printChanelStatus(1);
	printChanelStatus(2);

	// 2) get kd
	printChanelKD(0);
	printChanelKD(1);
	printChanelKD(2);

	// 3) print random numbers
	long maxRandom;
	printf("Enter max random number: ");
	scanf("%ld", &maxRandom);
	random(maxRandom);

	// 2) get kd
	printChanelKD(0);
	printChanelKD(1);
	printChanelKD(2);

	// 4) play sound
	doRock();

	return 0;
}