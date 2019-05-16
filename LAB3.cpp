#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <dos.h>

int attr = 0x6E;

void init();
void displayContents();
void charToString(unsigned char temp, char *str);
void directOutput(char* str, int x, int y, unsigned char attribute);

// ��������� �� ������ ����������� ���������� �� MASTER 
void interrupt(*old_0x08)(...);
void interrupt(*old_0x09)(...);
void interrupt(*old_0x0A)(...);
void interrupt(*old_0x0B)(...);
void interrupt(*old_0x0C)(...);
void interrupt(*old_0x0D)(...);
void interrupt(*old_0x0E)(...);
void interrupt(*old_0x0F)(...);

// ��������� �� ������ ����������� ���������� �� SLAVE
void interrupt(*old_0x70)(...);
void interrupt(*old_0x71)(...);
void interrupt(*old_0x72)(...);
void interrupt(*old_0x73)(...);
void interrupt(*old_0x74)(...);
void interrupt(*old_0x75)(...);
void interrupt(*old_0x76)(...);
void interrupt(*old_0x77)(...);


// ����� ����������� ���������� �� MASTER 

void interrupt new_0x08(...) { displayContents(); old_0x08(); }
void interrupt new_0x09(...) { attr ++; displayContents(); old_0x09(); }// ���������� ���������� 
void interrupt new_0x0A(...) { displayContents(); old_0x0A(); }
void interrupt new_0x0B(...) { displayContents(); old_0x0B(); }
void interrupt new_0x0C(...) { displayContents(); old_0x0C(); }
void interrupt new_0x0D(...) { displayContents(); old_0x0D(); }
void interrupt new_0x0E(...) { displayContents(); old_0x0E(); }
void interrupt new_0x0F(...) { displayContents(); old_0x0F(); }

// ����� ����������� ���������� �� SLAVE 
void interrupt new_0x70(...) { displayContents(); old_0x70(); }
void interrupt new_0x71(...) { displayContents(); old_0x71(); }
void interrupt new_0x72(...) { displayContents(); old_0x72(); }
void interrupt new_0x73(...) { displayContents(); old_0x73(); }
void interrupt new_0x74(...) { displayContents(); old_0x74(); }// ���������� ���� 
void interrupt new_0x75(...) { displayContents(); old_0x75(); }
void interrupt new_0x76(...) { displayContents(); old_0x76(); }
void interrupt new_0x77(...) { displayContents(); old_0x77(); }


int main()
{
	unsigned far *fp;
	int tsrSize;

	init();

	FP_SEG(fp) = _psp; //���������� ����� ����� PSP, ������ �������� ���������
	FP_OFF(fp) = 0x2c; // GET OFFSET OF DATA SEGMENT
	_dos_freemem(*fp); // FREE DATA SEGMENT
	
	tsrSize = (_DS - _CS) + (_SP / 16); //Ds-Cs - ������ ������� ������, ���������� ��� �������� ����, �.�.
									    //������� ������ ���������� ����� ����� �� ��������� ����
										//Sp - ������ ������� �������� ������
	_dos_keep(0, tsrSize + 1);//using for close program, params - size of program
							  //first param - ��� ����������	
							  //2nd param - ����� ������, ������� ������ ���� �������������� ��� ��������� ����� �� ����������.
							  //������������� �� ����������� ��� ���� �������� ������, ��������� � ���������� tsrSize ����� 1

	return 0;
}

void init()
{
	old_0x08 = getvect(0x08);//����������� ������ �������� ���������� �������� ����������
	old_0x09 = getvect(0x09);
	old_0x0A = getvect(0x0A);
	old_0x0B = getvect(0x0B);
	old_0x0C = getvect(0x0C);
	old_0x0D = getvect(0x0D);
	old_0x0E = getvect(0x0E);
	old_0x0F = getvect(0x0F);

	old_0x70 = getvect(0x70);
	old_0x71 = getvect(0x71);
	old_0x72 = getvect(0x72);
	old_0x73 = getvect(0x73);
	old_0x74 = getvect(0x74);
	old_0x75 = getvect(0x75);
	old_0x76 = getvect(0x76);
	old_0x77 = getvect(0x77);

	setvect(0x70, new_0x08);//��������� �������� ���������� �������� �������� 3
	setvect(0x71, new_0x09);
	setvect(0x72, new_0x0A);
	setvect(0x73, new_0x0B);
	setvect(0x74, new_0x0C);
	setvect(0x75, new_0x0D);
	setvect(0x76, new_0x0E);
	setvect(0x77, new_0x0F);

	setvect(0x08, new_0x70);
	setvect(0x09, new_0x71);
	setvect(0x0A, new_0x72);
	setvect(0x0B, new_0x73);
	setvect(0x0C, new_0x74);
	setvect(0x0D, new_0x75);
	setvect(0x0E, new_0x76);
	setvect(0x0F, new_0x77);

	_disable(); //��������� ���������� ����������

	//MASTER
	outp(0x20, 0x11);    //ICW1 - 00010001, 0 - ICW4 ON, 1 - USING SLAVE, 2 - SIZE OF VECTOR(1 - 4, 0 - 8) 3 - TRIGGER BY LEVEL
	outp(0x21, 0x70);    //ICW2 - 0x70 - high 5 bits - Base vector
	outp(0x21, 0x04);    //ICW3 - IRQ2 - SLAVE CONNECTED
	outp(0x21, 0x01);    //ICW4 - 1 - ��� ���������� - 8086 ON

	//SLAVE
	outp(0xA0, 0x11);	 //ICW1 - 00010001, 0 - ICW4 ON, 1 - USING SLAVE, 2 - SIZE OF VECTOR(1 - 4, 0 - 8) 3 - TRIGGER BY LEVEL	
	outp(0xA1, 0x08);	 //ICW2 - 0x08 - high 5 bits - Base vector
	outp(0xA1, 0x02);    //ICW3 - IRQ2 ��� �������� ����������� ������������ ������ ���� 0-2, ����� ������ ��������, � �������� ��������� �������
	outp(0xA1, 0x01);	 //ICW4 - 1 - ��� ���������� - 8086 ON

	_enable(); //����������� ��������� ���������� ����������
}

void displayContents()
{
	char str[9];

	directOutput("MASTER PIC ---  IMR: ", 0, 0, attr);
	charToString(inp(0x21), str);//������� �����, ��������� �������� � ����� 0x21h
	directOutput(str, 42, 0, attr);

	outp(0x20, 0x0A);//�������� ������� �������� MASTER
	directOutput("  |  IRR: ", 58, 0, attr);
	charToString(inp(0x20), str);
	directOutput(str, 78, 0, attr);

	outp(0x20, 0x0B);//�������� ������� ������������ MASTER
	directOutput("  |  ISR: ", 98, 0, attr);
	charToString(inp(0x20), str);
	directOutput(str, 114, 0, attr);


	directOutput("SLAVE  PIC ---  IMR: ", 0, 1, attr);
	charToString(inp(0xA1), str);//������� �����, ��������� �������� � ����� 0xA1h
	directOutput(str, 42, 1, attr);

	outp(0xA0, 0x0A);//�������� ������� �������� SLAVE
	directOutput("  |  IRR: ", 58, 1, attr);
	charToString(inp(0xA0), str);
	directOutput(str, 78, 1, attr);

	outp(0xA0, 0x0B);//�������� ������� ������������ SLAVE
	directOutput("  |  ISR: ", 98, 1, attr);
	charToString(inp(0xA0), str);
	directOutput(str, 114, 1, attr);
}

void charToString(unsigned char temp, char *str)
{
	int i;
	str[8] = 0;
	i = 7;
	while (temp)
	{
		str[i] = '0' + temp % 2;
		temp = temp / 2;
		i--;
	}
	for (; i > -1; i--)
	{
		str[i] = '0';
	}
}

void directOutput(char* str, int x, int y, unsigned char attribute)
{
	//���� �� ������ 0��8000000 �������� ��� ������� � ����� ������� ���� ������.��������� �� ��� ���� �������� 
	//���� ���� � ���� ���� ����� �������.� ������� �������� ����� ����� ���������� ���� ����, � ������� - ���� ����.
	char far* start = (char far*)0xb8000000; 
	start += x + 160 * y;

	int i = 0;
	while (str[i] != 0)
	{
		*start = str[i];
		start++;
		*start = attribute;
		start++;
		i++;
	}
}
