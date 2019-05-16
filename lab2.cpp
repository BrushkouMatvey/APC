#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 8
#define COLS 8
#define SIZE 64
#define TIMES 1000000



int main()
{
	srand(time(NULL));
	int flag = 1;
	clock_t begin, end;

	do
	{
		int arr[ROWS][COLS];
		int resultMatrix[ROWS][COLS];
		bool alreadyThere;
		int count = 0;
		int arr2[SIZE];

		for (int i = 0; i < SIZE;)
		{
			alreadyThere = false;
			int newRanomValue = rand() % 100;
			for (int j = 0; j < i; j++)
			{
				if (arr2[j] == newRanomValue)
				{
					alreadyThere = true;
					break;
				}
			}
			if (!alreadyThere)
			{
				arr2[i] = newRanomValue;
				i++;
			}
		}

		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				arr[i][j] = arr2[count];
				count++;
			}
		}

		printf("Ininitial matrix:\n");
		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COLS; j++)
				printf("%d\t ", arr[i][j]);
			printf("\n");
		}

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
				resultMatrix[i][j] = 0;


		long int result, sum;
		//C algorithm
		begin = clock();
		for (int num = 0; num < 1000000; num++)
		{
			sum = 0;
			for (int i = 0; i < ROWS; i++)
			{
				for (int j = 0; j < COLS; j++)
				{
					result = 0;
					result = pow(arr[i][j], 2);
					resultMatrix[i][j] = result;
					sum += result;
				}
			}
		}
		end = clock();
		printf("\nResult Matrix of C algorithm:\n");
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
				printf("%d\t ", resultMatrix[i][j]);
			printf("\n");
		}
		printf("time of C algorithm : %.6lf seconds\nresult:%d\n", (double)(end - begin) / CLOCKS_PER_SEC, sum);


		//asm algorithm
		begin = clock();
		for (int k = 0; k < TIMES; k++)
		{
			sum = 0;
			int col = 64;
			_asm
			{
				pusha //заносим все регистры в стек для созранения изначальных значений
				xor esi, esi //обнуляем регистр SI
				loop_asm : //метка цикла 
				mov eax, [arr + esi] //заносим в AX адрес элемента массива
					imul eax, [arr + esi] //возводим в квадрат
					mov[result + esi], eax //заносим результат в массив

					add sum, eax // добавляем результат в сумму
					add esi, 4 //увеличиваем SI на 4, чтобы переставить на другой элемент в массиве
					sub col, 1 // уменьшаем col(количство элементов в массиве
					jnz loop_asm // если не 0, перескакиваем на метку loop_1
					popa //если 0, достаем все регистры из стека
			}
		}
		end = clock();

		printf("\nResult Matrix of ASM algorithm:\n");
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
				printf("%d\t ", resultMatrix[i][j]);
			printf("\n");
		}
		printf("time of ASM algorithm : %.6lf seconds\nresult:%d\n", (double)(end - begin) / CLOCKS_PER_SEC, sum);

		//MMX algorithm
		begin = clock();
		for (int k = 0; k < TIMES; k++)
		{
			sum = 0;
			int col = 64;
			_asm
			{
				pusha
				xor esi, esi
				pxor MM7, MM7
				loop_mmx :
				movq MM0, [arr + esi]
					pmaddwd MM0, [arr + esi]
					movq[resultMatrix + esi], MM0
					paddd MM7, MM0
					add esi, 8
					sub col, 2
					jnz loop_mmx


					movq MM0, MM7
					psrlq MM7, 32
					paddd MM7, MM0
					movd sum, MM7
					emms
					popa
			}
		}
		end = clock();

		printf("\nResult Matrix of MMX algorithm:\n");
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
				printf("%d\t ", resultMatrix[i][j]);
			printf("\n");
		}
		printf("time of MMX algorithm : %.6lf seconds\nresult:%d\n", (double)(end - begin) / CLOCKS_PER_SEC, sum);


		printf("\nEnter 0 to exit from program or something else to continue ");
		rewind(stdin);
		scanf("%d", &flag);
	} while (flag);
	system("pause");
	return 0;
}