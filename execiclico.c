#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

struct Task
{
    char id[20];
    int periodo;
    int tempo_execucao;
    int prioridade;
};

int mdc(int a, int b)
{
    if (b == 0)
        return a;
    return mdc(b, a % b);
}

int mmc(int a, int b)
{
    return (a * b) / mdc(a, b);
}

void calcularCiclos(struct Task tasks[], int n, int *ciclo_primario, int *ciclo_secundario)
{
    *ciclo_primario = tasks[0].periodo;
    *ciclo_secundario = tasks[0].periodo;

    for (int i = 1; i < n; i++)
    {
        *ciclo_primario = mmc(*ciclo_primario, tasks[i].periodo);
        *ciclo_secundario = mdc(*ciclo_secundario, tasks[i].periodo);
    }
}

// Função de comparação para qsort
int compararPeriodo(const void *a, const void *b)
{
    const struct Task *taskA = (const struct Task *)a;
    const struct Task *taskB = (const struct Task *)b;
    return taskA->periodo - taskB->periodo;
}

void dividirTarefasEmCiclos(struct Task tasks[], int numTasks, int ciclo_primario, int ciclo_secundario)
{
    // Ordena as tarefas por período em ordem crescente
    qsort(tasks, numTasks, sizeof(struct Task), compararPeriodo);

    // Array para controlar se cada tarefa já foi executada em um ciclo secundário
    int *tarefaExecutada = (int *)calloc(numTasks, sizeof(int));

    // Quando tarefa executada pela sobra de tempo ha uma atualização do periodo,
    // logo vamos criar uma variavel auxiliar para ir moficiando o periodo ou nao
    int *novPeriodo = (int *)calloc(numTasks, sizeof(int));

    // preenchendo a variavel
    for (int j = 0; j < numTasks; j++)
    {
        novPeriodo[j] = tasks[j].periodo;
    }

    // Divide as tarefas em ciclos usando o ciclo secundário
    printf("\nEscalonamento Sugerido (Heurística: Highest Rate First - HRF):\n");
    printf("----------------------------------------------------------------------\n");
    int tempo_atual = 0;
    int ciclo_atual = 1;

    for (int ciclo = 1; ciclo <= ciclo_primario / ciclo_secundario; ciclo++)
    {
        printf("Ciclo Secundário %d:\n", ciclo);
        int ciclo_restante = ciclo_secundario;

        for (int i = 0; i < numTasks; i++)
        {
            // if (ciclo % (tasks[i].periodo / ciclo_secundario) == 0)
            //{
            if (tempo_atual % novPeriodo[i] == 0)
            {
                if (tasks[i].tempo_execucao <= ciclo_restante)
                {
                    printf("  - %s: tempo de execucao = %d, periodo = %d, prioridade = %d\n",
                           tasks[i].id, tasks[i].tempo_execucao, tasks[i].periodo, tasks[i].prioridade);
                    tarefaExecutada[i] = 1; // Marca a tarefa como executada
                    ciclo_restante -= tasks[i].tempo_execucao;
                }
            }
            //}
        }

        for (int k = 0; k < numTasks; k++)
        {
            if (!tarefaExecutada[k] && tasks[k].tempo_execucao <= ciclo_restante)
            {
                printf("  - %s: tempo de execucao = %d, periodo = %d, prioridade = %d (executada no tempo restante)\n",
                       tasks[k].id, tasks[k].tempo_execucao, tasks[k].periodo, tasks[k].prioridade);
                tarefaExecutada[k] = 1;                    // Marca a tarefa como executada
                ciclo_restante -= tasks[k].tempo_execucao; // Reduz o tempo restante no ciclo
                novPeriodo[k] = tasks[k].periodo + (ciclo-1) * ciclo_secundario;
            }
        }
        tempo_atual += ciclo_secundario;
        ciclo_atual++;
    }
}

int main()
{
    FILE *file = fopen("tarefas2.json", "r");
    if (!file)
    {
        printf("Erro ao abrir o arquivo JSON.\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    fread(fileContent, 1, fileSize, file);
    fclose(file);
    fileContent[fileSize] = '\0';

    json_object *root = json_tokener_parse(fileContent);
    if (!root)
    {
        printf("Erro ao analisar o arquivo JSON.\n");
        free(fileContent);
        return 1;
    }

    json_object *tasksJson = json_object_object_get(root, "tarefas");
    int numTasks = json_object_array_length(tasksJson);
    struct Task *tasks = malloc(numTasks * sizeof(struct Task));

    for (int i = 0; i < numTasks; i++)
    {
        json_object *taskJson = json_object_array_get_idx(tasksJson, i);
        json_object *idJson = json_object_object_get(taskJson, "id");
        json_object *periodoJson = json_object_object_get(taskJson, "periodo");
        json_object *tempoExecucaoJson = json_object_object_get(taskJson, "tempo_execucao");
        json_object *prioridadeJson = json_object_object_get(taskJson, "prioridade");

        strcpy(tasks[i].id, json_object_get_string(idJson));
        tasks[i].periodo = json_object_get_int(periodoJson);
        tasks[i].tempo_execucao = json_object_get_int(tempoExecucaoJson);
        tasks[i].prioridade = json_object_get_int(prioridadeJson);
    }

    // Cálculo dos ciclos primário e secundário
    int ciclo_primario, ciclo_secundario;
    calcularCiclos(tasks, numTasks, &ciclo_primario, &ciclo_secundario);

    // Impressão dos ciclos calculados
    printf("Calculo de Ciclos para o Executivo Ciclico:\n");
    printf("-------------------------------------------\n");
    printf("Tempo de Ciclo Primario: %d unidades de tempo\n", ciclo_primario);
    printf("Tempo de Ciclo Secundario: %d unidades de tempo\n", ciclo_secundario);

    // Divisão das tarefas em ciclos
    dividirTarefasEmCiclos(tasks, numTasks, ciclo_primario, ciclo_secundario);

    // Liberação de memória alocada
    json_object_put(root);
    free(fileContent);
    free(tasks);

    return 0;
}
