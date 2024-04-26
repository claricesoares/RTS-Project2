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

// Função que calcula MDC
int mdc(int a, int b)
{
    if (b == 0)
        return a;
    return mdc(b, a % b);
}

// Função que calcula MMC
int mmc(int a, int b)
{
    return (a * b) / mdc(a, b);
}

// Função que calcula o tempo do ciclo maior e do ciclo menor
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

    // Array que controla se cada tarefa já foi executada em um ciclo secundário
    int *tarefaExecutada = (int *)calloc(numTasks, sizeof(int));

    // Quando tarefa é executada pela sobra de tempo, ocorre uma atualização do periodo,
    // Assim, é criada uma variável auxiliar para modificar ou não o periodo
    int *novPeriodo = (int *)calloc(numTasks, sizeof(int));

    // Preenche a variável auxiliar do período
    for (int j = 0; j < numTasks; j++)
    {
        novPeriodo[j] = tasks[j].periodo;
    }

    // Divide as tarefas em ciclos usando o ciclo secundário
    printf("\nEscalonamento Sugerido (Heurística: Highest Rate First - HRF):\n");
    printf("----------------------------------------------------------------------\n");
    int tempo_atual = 0;
    int ciclo_atual = 1;

    // LAço de execução para cada ciclo menor
    for (int ciclo = 1; ciclo <= ciclo_primario / ciclo_secundario; ciclo++)
    {
        printf("Ciclo Secundário %d:\n", ciclo);
        int ciclo_restante = ciclo_secundario;

        for (int i = 0; i < numTasks; i++)
        {
            // Para cada tarefa, verifica se está no período de execução e se há tempo de processamento suficiente
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
            // Para cada tarefa, verifica se há tempo hábil para execução
            else if (!tarefaExecutada[i] && tasks[i].tempo_execucao <= ciclo_restante)
            {
                printf("  - %s: tempo de execucao = %d, periodo = %d, prioridade = %d (executada no tempo restante)\n",
                       tasks[i].id, tasks[i].tempo_execucao, tasks[i].periodo, tasks[i].prioridade);
                tarefaExecutada[i] = 1;                    // Marca a tarefa como executada
                ciclo_restante -= tasks[i].tempo_execucao; // Reduz o tempo restante no ciclo
                novPeriodo[i] = tasks[i].periodo + (ciclo-1) * ciclo_secundario;
            }
        }

        // Atualiza o tempo e ciclo atual
        tempo_atual += ciclo_secundario;
        ciclo_atual++;
    }
}


// Função que calcula a viabilidade de escalonamento com base na utilização das tarefas definidas
float calcularTaxaUtilizacao(struct Task tasks[], int numTasks)
{
    // Taxa de utilização total
    float utilizacao_total = 0.0;

    // Calcula a taxa de utilização de cada tarefa e adiciona à utilização total
    for (int i = 0; i < numTasks; i++)
    {
        float utilizacao_tarefa = (float)tasks[i].tempo_execucao / (float)tasks[i].periodo;
        utilizacao_total += utilizacao_tarefa;
    }

    // Imprime a taxa de utilização total do sistema
    printf("Verificação de Escalonabilidade:\n");
    printf("--------------------------------\n");
    printf("Utilizacao: %.2f\n", utilizacao_total);

    // Verifica se a taxa de utilização total é menor ou igual a 1 (viabilidade)
    if (utilizacao_total <= 1.0)
    {
        printf("Escalonamento viavel.\n\n");
    }
    else
    {
        printf("Escalonamento nao viavel. Abortando calculos adicionais.\n\n");
    }

    return utilizacao_total;
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

    // Cálculo da taxa de utilização total e verificação de viabilidade
    float utilizacao_total = calcularTaxaUtilizacao(tasks, numTasks);
    // Caso a utilização > 1, o escalonamento não é viável, e os calculos são abortados
    if (utilizacao_total > 1.0) {
        printf("Escalonamento nao viavel.\n");
        // Liberação de memória alocada
        json_object_put(root);
        free(fileContent);
        free(tasks);
        return 1;
    } else {
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

    return 0;
}
