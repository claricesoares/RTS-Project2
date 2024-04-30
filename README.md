# RTS Project 2: Executivo Cíclico

## Objetivo
Este projeto visa criar um sistema de executivo cíclico para calcular os tempos de ciclo primário e secundário para um conjunto de tarefas, além de utilizar uma heurística para sugerir um escalonamento das mesmas dentro desses ciclos.

## Funcionalidades
- **Cálculo dos tempos de ciclo:**
    - Tempo de ciclo primário: calculado como o mínimo múltiplo comum (MMC) dos períodos das tarefas, representando o ciclo maior que sincroniza todas as tarefas em sua periodicidade.
    - Tempo de ciclo secundário: determinado como o maior divisor comum (MDC) dos períodos das tarefas, representando o menor ciclo possível que ainda respeita as periodicidades das tarefas.

- **Cálculo do escalonamento usando heurísticas:**
    - Após determinar os tempos de ciclo, o sistema aplica uma heurística (neste caso, Maior Taxa de Periodicidade Primeiro) para sugerir um escalonamento otimizado das tarefas dentro do ciclo determinado.
    - A heurística considera os tempos de execução das tarefas, seus períodos e prioridades para organizar a sequência de execução dentro do ciclo.
    - As heurísticas de ordem de tarefas em um executivo cíclico são métodos práticos para determinar a sequência de execução dentro de cada ciclo (frame), visando otimizar a performance do sistema e garantir que todas as tarefas sejam atendidas dentro de seus períodos.

## Maior Taxa de Periodicidade Primeiro (HRF)
Nesta abordagem, as tarefas são priorizadas com base na frequência de sua execução; tarefas que devem ser executadas mais frequentemente têm prioridade. Isso é baseado na lógica de que tarefas com períodos mais curtos são geralmente mais críticas e, portanto, devem ser atendidas com maior prioridade para evitar a violação de seus prazos.

## Requisitos
- **Requisito 1**
    - O tamanho do frame deve ser maior ou igual ao maior tempo de execução de uma tarefa. Em outras palavras:
    - 𝑓 ≥ max(e𝑖)
    - Onde 𝑓 é o tamanho do frame e e𝑖 o tempo de execução.
- **Requisito 2**
    - O tamanho de frames candidatos devem caber igualmente dentro de um ciclo maior.
- **Requisito 3**
    - Deve existir um frame entre o release-time (𝑡′) e o deadline (𝑡′ + 𝐷𝑖) de todos os jobs. Em outras palavras:
    - 2𝑓 – mdc(P𝑖, 𝑓) ≤ 𝐷𝑖
    - Onde 𝑓 é o tamanho do frame, P𝑖 o período de uma task e 𝐷𝑖 a deadline.

## Sobre o JSON
Para manipulação de JSON, uma tarefa comum em configurações, comunicação entre sistemas ou interfaces de programação de aplicativos, foi utilizada a biblioteca json-c. Esta biblioteca fornece uma implementação completa de JSON com funções para analisar strings JSON e formatar dados C para strings JSON. Ela é adequada para integrar funcionalidades JSON em aplicativos C ou para trabalhar com JSON recebido de APIs e serviços externos.

## Instruções
1. Instalar a biblioteca json-c
```bash
sudo apt-get install libjson-c-dev
```

2. Compile o arquivo execiclico.c usando o GCC

```bash
gcc execiclico.c -o execiclico -ljson-c
```

3. Execute o arquivo

```bash
./execiclico
```
