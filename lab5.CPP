#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

unsigned long msTimer = 0;

int hour;
int min;
int sec;
int dayOfWeek;
int dayOfMonth;
int month;
int year;

int setHour;
int setMin;
int setSec;
int setDayOfWeek;
int setDayOfMonth;
int setMonth;
int setYear;

int alarmHour;
int alarmMin;
int alarmSec;

int value;
int input;

void interrupt(*oldAlarm)(...);// Переменная для хранения старого вектора будильника
void interrupt newAlarm(...);// Прототип программы-обработчика прерывания будильника


int intToBCD(int number);

int BCDToInt(int number);

void setDelay();
void setAlarm();
void showAlarm();
void showTime()
{
	outp(0x70, 4); //регистр 4h (часы)
	hour = inp(0x71);
	outp(0x70, 2);//регистр 2h (минуты)
	min = inp(0x71);
	outp(0x70, 0);//регистр 0h (секунды)
	sec = inp(0x71);
	printf("%d:%d:%d\n", BCDToInt(hour), BCDToInt(min), BCDToInt(sec));
}

void setTime()
{
	do
	{
		outp(0x70, 0x0A);
	} while (inp(0x71) & 0x80); //1 - прерывния разрешены, 0 - запрещены
	outp(0x70, 0x0B);
	value = inp(0x71) | 0x80;//7 бит - установка часов
	outp(0x70, 0x0B);
	outp(0x71, value);

	printf("Enter time\n");
	printf("Hours:");
	scanf("%d", &setHour);
	printf("Minutes:");
	scanf("%d", &setMin);
	printf("Seconds:");
	scanf("%d", &setSec);

	outp(0x70, 0x04);
	outp(0x71, intToBCD(setHour));//устанавливаем часы
	outp(0x70, 0x02);
	outp(0x71, intToBCD(setMin));//устанавливаем минуты
	outp(0x70, 0x00);
	outp(0x71, intToBCD(setSec));//устанавливаем секунды

	do
	{
		outp(0x70, 0x0A);
	} while (inp(0x71) & 0x80); //Этот бит управляет режимом немаскируемых прерываний(1 - прерывния разрешены, 0 - запрещены)
	outp(0x70, 0x0B);
	value = inp(0x71) & 0x7F;//обнуление 7 бита регистра B
	outp(0x71, value);//сброс бита установки времени
}

void main()
{
	oldAlarm = getvect(0x4A); //Это прерывание вызывается при срабатывании будильника
	setvect(0x4A, newAlarm); //устанавливаем новый вектор
	input = 1;
	while (input != 0)
	{
		printf("1.Get time\n2.Set time\n3.Set alarm\n0.Exit\n");
		input = getch() - '0';
		switch (input)
		{
		case 1:
		{
			showTime();
			break;
		}
		case 2:
		{
			setTime();
			break;
		}
		case 3:
		{
			setAlarm();
			break;
		}
		}
	}
	setvect(0x4A, oldAlarm);//устанавливаем старый вектор
}

int BCDToInt(int number)
{
	return (((number / 16) * 10) + (number % 16));
}

int intToBCD(int number)
{	
	return (((number / 10) * 16) + (number % 10));
}

void interrupt newAlarm(...)
{
	unsigned char attribute = 0x07;
	int i = 0;
	char str[] = "ALARM IS HANDLED";
	clrscr();
	char far* start = (char far*)0xb8000000;

	while (str[i] != 0)
	{
		*start = str[i];
		start++;
		*start = attribute;
		start++;
		i++;
	}

}

void setAlarm()
{
	printf("Enter time\n");
	printf("Hours:");
	scanf("%d", &alarmHour);
	printf("Minutes:");
	scanf("%d", &alarmMin);
	printf("Seconds:");
	scanf("%d", &alarmSec);

	value = inp(0xA1);
	outp(0xA1, value & 0xFE);

	do
	{
		outp(0x70, 0x0A);
	} while (inp(0x71) & 0x80);//Этот бит управляет режимом немаскируемых прерываний(1 - прерывния разрешены, 0 - запрещены)
	outp(0x70, 0x0B);
	value = inp(0x71) | 0x80;//запрещение обновления часов
	outp(0x70, 0x0B);
	outp(0x71, value);

	outp(0x70, 0x0B);
	value = inp(0x71);
	outp(0x70, 0x0B);
	outp(0x71, value | 0x20);//разрешение прерывания от будильника

	outp(0x70, 0x05);
	outp(0x71, intToBCD(alarmHour));//устанавливаем часы будильника
	outp(0x70, 0x03);
	outp(0x71, intToBCD(alarmMin));//устанавливаем минуты будильника
	outp(0x70, 0x01);
	outp(0x71, intToBCD(alarmSec));//устанавливаем часы секунды будильника

	do
	{
		outp(0x70, 0x0A);
	} while (inp(0x71) & 0x80);
	outp(0x70, 0x0B);
	value = inp(0x71) & 0x7F;
	outp(0x71, value);
}