#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

int main()
{
	int flag = 1;
	do
	{
		system("cls");
		float border1, border2, step;
		float result;

		printf("Enter first border:");
		rewind(stdin);
		scanf("%f", &border1);
		printf("Enter second border:");
		rewind(stdin);
		scanf("%f", &border2);
		printf("Enter step:");
		rewind(stdin);
		scanf("%f", &step);


		clock_t begin = clock();
		for (double i = border1; i < border2; i += step)
		{
			result = tan(i + 1) / sin(i);
		}
		clock_t end = clock();

		printf("time(C algorithm) : %.6lf seconds\nresult:%lf\n\n", (double)(end - begin) / CLOCKS_PER_SEC, result);

		double res;
		begin = clock();
		_asm finit;
		for (double i = border1; i < border2; i += step)
		{
			res = i;

			_asm
			{
				fld1
				fadd res
				fptan
				fdiv

				fld res
				fsin

				fdiv
				fstp res
			}
			//std::cout << res << std::endl; /*std::endl*/;
		}
		_asm fwait;
		end = clock();

		printf("time(coprocessor algorithm) : %.6lf seconds\nresult:%lf", (double)(end - begin) / CLOCKS_PER_SEC, res);
		printf("\nEnter 0 to exit from program or something else to continue");
		rewind(stdin);
		scanf("%d", &flag);
	} while (flag);



	system("pause");
	return 0;
}