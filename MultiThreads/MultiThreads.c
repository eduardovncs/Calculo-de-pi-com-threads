#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <windows.h>

void WINAPI calcularPi(long long *limite);
void sincronizarThreads();
void executarThreads();
void comecarContagem();
double finalizarContagem();

double PCFreq;
__int64 CounterStart;

double valorPi = 0;
int numeroThreads;
long quantidadeRepeticoes;
long long **contador;

int main() 
{
	double tempoExecucao;

	//Leitura dos valores

	printf("Numero de Threads: ");
	scanf("%d", &numeroThreads);
	printf("Quantidade de repeticoes: ");
	scanf("%ld", &quantidadeRepeticoes);
	
	if(numeroThreads > quantidadeRepeticoes)
		return EXIT_FAILURE;

	//Alocação de memória para sincronização das Threads

	contador = (long long**)malloc(numeroThreads * sizeof(long long*));
	if (contador == NULL)
		return EXIT_FAILURE;
	for (int i = 0; i < numeroThreads; i++) { 
		contador[i] = (long long*)malloc(2 * sizeof(long long*));
		if(contador[i] == NULL) 
			return EXIT_FAILURE;
	}

	sincronizarThreads();

	comecarContagem();
	executarThreads();
	tempoExecucao = finalizarContagem();

	printf("\n Valor de pi: %.40Lf \n Tempo execucao: %.5f segundos\n", valorPi, tempoExecucao);
	system("pause");
	free(contador);
	return EXIT_SUCCESS;
}


void WINAPI calcularPi(long long *limite)
{
	int numerador = 1;
	double valor = 0;
	for (long long i = limite[0]; i <= limite[1]; i += 2) {
		valor += numerador * 4.0/i;
		numerador *= -1;
	}
	valorPi += valor;
}

void sincronizarThreads()
{
	long long aux = -1;
	for (int i = 0; i < numeroThreads; i++) {
		contador[i][0] = aux + 2;
		
		aux += 4 * (long long)round((double)quantidadeRepeticoes / numeroThreads);

		if (numeroThreads - i == 1) {
			aux = (long long)(1 + ((double)quantidadeRepeticoes * 2 - 1) * 2);
		}
		contador[i][1] = aux;
	}
}

void executarThreads()
{
	HANDLE *thread = (HANDLE*)malloc(numeroThreads * sizeof(HANDLE));

	for (int i = 0; i < numeroThreads; i++)
	{
		thread[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)calcularPi, contador[i], 0, NULL);
	}
	printf("\n Threads Inicizadas!\n");

	WaitForMultipleObjects(numeroThreads, thread, TRUE, INFINITE);
}

void comecarContagem()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		exit(EXIT_FAILURE);
	PCFreq = (double)(li.QuadPart) / 1000.0;
	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}

double finalizarContagem()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (double)((li.QuadPart - CounterStart) / PCFreq)/1000;
}