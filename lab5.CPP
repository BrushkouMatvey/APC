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

void interrupt(*oldAlarm)(...);// ���������� ��� �������� ������� ������� ����������
void interrupt newAlarm(...);// �������� ���������-����������� ���������� ����������


int intToBCD(int number);

int BCDToInt(int number);

void setDelay();
void setAlarm();
void showAlarm();
void showTime()
{
	outp(0x70, 4); //������� 4h (����)
	hour = inp(0x71);
	outp(0x70, 2);//������� 2h (������)
	min = inp(0x71);
	outp(0x70, 0);//������� 0h (�������)
	sec = inp(0x71);
	printf("%d:%d:%d\n", BCDToInt(hour), BCDToInt(min), BCDToInt(sec));
}

void setTime()
{
	do
	{
		outp(0x70, 0x0A);
	} while (inp(0x71) & 0x80); //1 - ��������� ���������, 0 - ���������
	outp(0x70, 0x0B);
	value = inp(0x71) | 0x80;//7 ��� - ��������� �����
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
	outp(0x71, intToBCD(setHour));//������������� ����
	outp(0x70, 0x02);
	outp(0x71, intToBCD(setMin));//������������� ������
	outp(0x70, 0x00);
	outp(0x71, intToBCD(setSec));//������������� �������

	do
	{
		outp(0x70, 0x0A);
	} while (inp(0x71) & 0x80); //���� ��� ��������� ������� ������������� ����������(1 - ��������� ���������, 0 - ���������)
	outp(0x70, 0x0B);
	value = inp(0x71) & 0x7F;//��������� 7 ���� �������� B
	outp(0x71, value);//����� ���� ��������� �������
}

void main()
{
	oldAlarm = getvect(0x4A); //��� ���������� ���������� ��� ������������ ����������
	setvect(0x4A, newAlarm); //������������� ����� ������
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
	setvect(0x4A, oldAlarm);//������������� ������ ������
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
	} while (inp(0x71) & 0x80);//���� ��� ��������� ������� ������������� ����������(1 - ��������� ���������, 0 - ���������)
	outp(0x70, 0x0B);
	value = inp(0x71) | 0x80;//���������� ���������� �����
	outp(0x70, 0x0B);
	outp(0x71, value);

	outp(0x70, 0x0B);
	value = inp(0x71);
	outp(0x70, 0x0B);
	outp(0x71, value | 0x20);//���������� ���������� �� ����������

	outp(0x70, 0x05);
	outp(0x71, intToBCD(alarmHour));//������������� ���� ����������
	outp(0x70, 0x03);
	outp(0x71, intToBCD(alarmMin));//������������� ������ ����������
	outp(0x70, 0x01);
	outp(0x71, intToBCD(alarmSec));//������������� ���� ������� ����������

	do
	{
		outp(0x70, 0x0A);
	} while (inp(0x71) & 0x80);
	outp(0x70, 0x0B);
	value = inp(0x71) & 0x7F;
	outp(0x71, value);
}