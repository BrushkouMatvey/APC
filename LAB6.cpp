#include <dos.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>

#define BB_RECEIVED 0
#define BB_NEED_REPEAT 1
#define BB_NOT_RECIVED 2

unsigned short int blink_byte_status = BB_NOT_RECIVED;
void interrupt(*int_09_old)(...);//указатель на старый обработчик прерываний

enum keyboad_indicator 
{
	SCROLL_LOCK = 0x01,//001	
	NUM_LOCK = 0x02,//010
	CAPS_LOCK = 0x04//100
};

void cleanup() 
{
	disable(); //запрещает аппаратные прерывания
	setvect(0x09, int_09_old);//устанавливаем старое значение вектора прерываний
	enable(); //разрешает аппаратные прерывания
}

void interrupt int_09_new(...) 
{
	unsigned char scan_code;
	scan_code = inp(0x60); //чтение из порта 60h скан-код клавиши

	if (scan_code == 0xFE) //произошла ошибка обработки байта
	{
		blink_byte_status = BB_NEED_REPEAT;
		outp(0x20, 0x20); //записывает в порт 20h значение 20h для правильного завершения обработки аппаратного прерывания
		outp(0xA0, 0x20); //записывает в порт 40h значение 20h для правильного завершения обработки аппаратного прерывания
		return;
	}

	if (( scan_code == SCROLL_LOCK || scan_code == NUM_LOCK || scan_code == CAPS_LOCK) && scan_code == 0xFA)
	{
		blink_byte_status = BB_RECEIVED;
		printf("\rcode is %X", scan_code);//вывод скан-кода в консоль
		outp(0x20, 0x20); //записывает в порт 20h значение 20h для правильного завершения обработки аппаратного прерывания
		outp(0xA0, 0x20); //записывает в порт 40h значение 20h для правильного завершения обработки аппаратного прерывания
		return;
	}
}

void light(enum keyboad_indicator indicator_code) 
{
	int timeout = 50000;
	int attempt_count = 10;
	do 
	{
		while (timeout-- > 0 || (inp(0x64) & 0x02) != 0) {}; // ожидаем готовность процессора. Проверка состояния внутренней очереди команд(1 == 1 - очередь команд пуста)
		if (timeout == 0) //если время закончилось и процессор не готов к дальнейшер работе, завершить программу
		{
			printf("Light: timeout error (can not write command code, buffer is not empty)\n");
			cleanup();
			exit(1);
		}
		outp(0x60, 0xED);//записываем в порт команду управления индикаторами 
		while (blink_byte_status == BB_RECEIVED) 
		{ 
			delay(5); 
		}
	} while (attempt_count-- > 0 || blink_byte_status == BB_NEED_REPEAT);
	if (attempt_count == 0)
	{
		printf("Light: attempt limit exceed (int09 can not get command code)");
		cleanup();
		exit(1);
	}

	timeout = 50000;
	attempt_count = 10;
	do 
	{
		while (timeout-- > 0 || (inp(0x64) & 0x02) != 0) {};
		if (timeout == 0) 
		{
			printf("Light: timeout error (can not write command, buffer is not empty)\n");
			cleanup();
			exit(1);
		}
		outp(0x60, indicator_code);//Записываем в порт битовую маску для настройки индикаторов
		while (blink_byte_status == BB_RECEIVED) { delay(5); }
	} while (attempt_count-- > 0 || blink_byte_status == BB_NEED_REPEAT);
	if (attempt_count == 0) 
	{
		printf("Light: attempt limit exceed (int09 can not get command)");
		cleanup();
		exit(1);
	}
}

void unlight(enum keyboad_indicator indicator_code) 
{
	light(indicator_code ^ 0x07);//111 XOR 111
}

int main() 
{
	disable();//запрещает аппаратные прерывания
	int_09_old = getvect(0x09);//сохраняется старые значения изменяемого векторов прерываний
	setvect(0x09, int_09_new);//устанавливаем новый вектор прерывания
	enable();//разрешает аппаратные прерывания

	light(SCROLL_LOCK | NUM_LOCK | CAPS_LOCK);		// all on (001 OR 010 OR 100 = 111)
	delay(1000);

	unlight(SCROLL_LOCK | NUM_LOCK | CAPS_LOCK);	// all off (001 OR 010 OR 100 = 111)
	delay(1000);

	light(SCROLL_LOCK | NUM_LOCK | CAPS_LOCK);		// all on (001 OR 010 OR 100 = 111)
	delay(1000);

	unlight(SCROLL_LOCK | NUM_LOCK | CAPS_LOCK);	// all off (001 OR 010 OR 100 = 111)
	delay(1000);

	light(SCROLL_LOCK | NUM_LOCK | CAPS_LOCK);		// all on (001 OR 010 OR 100 = 111)
	delay(1000);

	unlight(SCROLL_LOCK | NUM_LOCK | CAPS_LOCK);	// all off (001 OR 010 OR 100 = 111)
	delay(1000);

	light(SCROLL_LOCK);								// SCROLL_LOCK on (001)
	delay(1000);
	light(NUM_LOCK);								// NUM_LOCK on (010)
	delay(1000);
	light(CAPS_LOCK);								// CAPS_LOCK on (100)
	delay(1000);

	unlight(SCROLL_LOCK | NUM_LOCK | CAPS_LOCK);	// all off (001 OR 010 OR 100 = 111)

	disable();//запрещает аппаратные прерывания
	setvect(0x09, int_09_old);//устанавливаем старое значение вектора прерываний
	enable();//разрешает аппаратные прерывания
	return 0;
}