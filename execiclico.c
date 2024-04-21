#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

struct Task {
    char id[20];
    int periodo;
    int tempo_execucao;
    int prioridade;
};

int mdc(int a, int b) {
    if (b == 0)
        return a;
    return mdc(b, a % b);
}

int mmc(int a, int b) {
    return (a * b) / mdc(a, b);
}

void calcularCiclos(struct Task tasks[], int n, int *ciclo_primario, int *ciclo_secundario) {
    *ciclo_primario = tasks[0].periodo;
    *ciclo_secundario = tasks[0].periodo;

    for (int i = 1; i < n; i++) {
        *ciclo_primario = mmc(*ciclo_primario, tasks[i].periodo);
        *ciclo_secundario = mdc(*ciclo_secundario, tasks[i].periodo);
    }
}

// Função de comparação para qsort
int compararPeriodo(const void *a, const void *b) {
    const struct Task *taskA = (const struct Task *)a;
    const struct Task *taskB = (const struct Task *)b;
    return taskB->periodo - taskA->periodo; // Ordem decrescente por período
}

void dividirTarefasEmCiclos(struct Task tasks[], int numTasks, int ciclo_primario) {
    // Ordena as tarefas por período em ordem decrescente
    qsort(tasks, numTasks, sizeof(struct Task), compararPeriodo);

    // Divide as tarefas em ciclos
    printf("\nEscalonamento Sugerido (Heuristica: Maior Taxa de Periodicidade Primeiro - HRF):\n");
    printf("----------------------------------------------------------------------\n");
    int ciclo_atual = 1;
    int tempo_atual = 0;
    for (int i = 0; i < numTasks; i++) {
        if (tempo_atual + tasks[i].tempo_execucao <= ciclo_primario) {
            printf("Ciclo %d:\n", ciclo_atual);
            printf("  - %s: tempo de execucao = %d, periodo = %d, prioridade = %d\n",
                   tasks[i].id, tasks[i].tempo_execucao, tasks[i].periodo, tasks[i].prioridade);
            tempo_atual += tasks[i].tempo_execucao;
        } else {
            ciclo_atual++;
            printf("\nCiclo %d:\n", ciclo_atual);
            printf("  - %s: tempo de execucao = %d, periodo = %d, prioridade = %d\n",
                   tasks[i].id, tasks[i].tempo_execucao, tasks[i].periodo, tasks[i].prioridade);
            tempo_atual = tasks[i].tempo_execucao;
        }
    }
}

int main() {
    FILE *file = fopen("tarefas.json", "r");
    if (!file) {
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
    if (!root) {
        printf("Erro ao analisar o arquivo JSON.\n");
        free(fileContent);
        return 1;
    }

    json_object *tasksJson = json_object_object_get(root, "tarefas");
    int numTasks = json_object_array_length(tasksJson);
    struct Task *tasks = malloc(numTasks * sizeof(struct Task));

    for (int i = 0; i < numTasks; i++) {
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

    int ciclo_primario, ciclo_secundario;
    calcularCiclos(tasks, numTasks, &ciclo_primario, &ciclo_secundario);

    printf("Calculo de Ciclos para o Executivo Ciclico:\n");
    printf("-------------------------------------------\n");
    printf("Tempo de Ciclo Primario: %d unidades de tempo\n", ciclo_primario);
    printf("Tempo de Ciclo Secundario: %d unidades de tempo\n", ciclo_secundario);

    dividirTarefasEmCiclos(tasks, numTasks, ciclo_primario);

    json_object_put(root);
    free(fileContent);
    free(tasks);

    return 0;
}