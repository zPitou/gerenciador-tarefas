/*
 * Gerenciador de Tarefas
 * Estruturas usadas:
 *   - Fila: armazena tarefas pendentes (FIFO - primeira a entrar, primeira a sair)
 *   - Pilha: armazena tarefas concluidas para poder desfazer (LIFO - ultima a entrar, primeira a sair)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESCRICAO 200
#define ARQUIVO "tarefas.csv"

/* ============================================================
   DEFINICAO DOS NOS E ESTRUTURAS
   ============================================================ */

/* No usado tanto pela fila quanto pela pilha */
typedef struct No {
    char descricao[MAX_DESCRICAO];
    struct No *proximo;
} No;

/* Fila de tarefas pendentes */
typedef struct {
    No *inicio;  /* de onde retiramos (frente da fila) */
    No *fim;     /* onde inserimos (fundo da fila)      */
    int tamanho;
} Fila;

/* Pilha de tarefas concluidas (para desfazer) */
typedef struct {
    No *topo;
    int tamanho;
} Pilha;

/* ============================================================
   OPERACOES DA FILA
   ============================================================ */

void fila_inicializar(Fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->tamanho = 0;
}

/* Adiciona tarefa no fim da fila */
void fila_enfileirar(Fila *f, const char *descricao) {
    No *novo = (No *)malloc(sizeof(No));
    if (novo == NULL) {
        printf("Erro: memoria insuficiente.\n");
        return;
    }
    strncpy(novo->descricao, descricao, MAX_DESCRICAO - 1);
    novo->descricao[MAX_DESCRICAO - 1] = '\0';
    novo->proximo = NULL;

    if (f->fim == NULL) {
        /* fila estava vazia */
        f->inicio = novo;
        f->fim = novo;
    } else {
        f->fim->proximo = novo;
        f->fim = novo;
    }
    f->tamanho++;
}

/* Remove e retorna a tarefa do inicio da fila */
No *fila_desenfileirar(Fila *f) {
    if (f->inicio == NULL) return NULL;

    No *removido = f->inicio;
    f->inicio = f->inicio->proximo;
    if (f->inicio == NULL) f->fim = NULL; /* fila ficou vazia */
    removido->proximo = NULL;
    f->tamanho--;
    return removido;
}

/* ============================================================
   OPERACOES DA PILHA
   ============================================================ */

void pilha_inicializar(Pilha *p) {
    p->topo = NULL;
    p->tamanho = 0;
}

/* Empilha uma tarefa concluida */
void pilha_empilhar(Pilha *p, const char *descricao) {
    No *novo = (No *)malloc(sizeof(No));
    if (novo == NULL) {
        printf("Erro: memoria insuficiente.\n");
        return;
    }
    strncpy(novo->descricao, descricao, MAX_DESCRICAO - 1);
    novo->descricao[MAX_DESCRICAO - 1] = '\0';
    novo->proximo = p->topo;
    p->topo = novo;
    p->tamanho++;
}

/* Desempilha e retorna a tarefa mais recente */
No *pilha_desempilhar(Pilha *p) {
    if (p->topo == NULL) return NULL;

    No *removido = p->topo;
    p->topo = p->topo->proximo;
    removido->proximo = NULL;
    p->tamanho--;
    return removido;
}

/* ============================================================
   PERSISTENCIA EM ARQUIVO
   ============================================================ */

/*
 * Formato do arquivo tarefas.csv:
 *   status,descricao
 *   pendente,Estudar para a prova
 *   concluida,Comprar po
 */

void salvar_arquivo(Fila *f, Pilha *p) {
    FILE *arq = fopen(ARQUIVO, "w");
    if (arq == NULL) {
        printf("Erro ao abrir arquivo para salvar.\n");
        return;
    }

    /* Salva tarefas pendentes (percorre a fila sem modificar) */
    No *atual = f->inicio;
    while (atual != NULL) {
        fprintf(arq, "pendente,%s\n", atual->descricao);
        atual = atual->proximo;
    }

    /* Salva tarefas concluidas (percorre a pilha sem modificar) */
    atual = p->topo;
    while (atual != NULL) {
        fprintf(arq, "concluida,%s\n", atual->descricao);
        atual = atual->proximo;
    }

    fclose(arq);
    printf("Dados salvos em '%s'.\n", ARQUIVO);
}

void carregar_arquivo(Fila *f, Pilha *p) {
    FILE *arq = fopen(ARQUIVO, "r");
    if (arq == NULL) {
        /* Primeira execucao: arquivo nao existe ainda, tudo bem */
        return;
    }

    char linha[MAX_DESCRICAO + 20];
    while (fgets(linha, sizeof(linha), arq)) {
        /* Remove o '\n' do final da linha */
        linha[strcspn(linha, "\n")] = '\0';

        /* Separa status e descricao pela primeira virgula */
        char *virgula = strchr(linha, ',');
        if (virgula == NULL) continue;

        *virgula = '\0';
        char *status = linha;
        char *descricao = virgula + 1;

        if (strcmp(status, "pendente") == 0) {
            fila_enfileirar(f, descricao);
        } else if (strcmp(status, "concluida") == 0) {
            pilha_empilhar(p, descricao);
        }
    }

    fclose(arq);
}

/* ============================================================
   FUNCOES DO MENU
   ============================================================ */

void adicionar_tarefa(Fila *f) {
    char descricao[MAX_DESCRICAO];
    printf("Digite a descricao da tarefa: ");
    fgets(descricao, sizeof(descricao), stdin);
    descricao[strcspn(descricao, "\n")] = '\0'; /* remove '\n' */

    if (strlen(descricao) == 0) {
        printf("Descricao vazia. Tarefa nao adicionada.\n");
        return;
    }

    fila_enfileirar(f, descricao);
    printf("Tarefa adicionada com sucesso!\n");
}

void listar_pendentes(Fila *f) {
    if (f->inicio == NULL) {
        printf("Nenhuma tarefa pendente.\n");
        return;
    }

    printf("\n--- Tarefas Pendentes (%d) ---\n", f->tamanho);
    No *atual = f->inicio;
    int numero = 1;
    while (atual != NULL) {
        printf("%d. %s\n", numero++, atual->descricao);
        atual = atual->proximo;
    }
}

void listar_concluidas(Pilha *p) {
    if (p->topo == NULL) {
        printf("Nenhuma tarefa concluida.\n");
        return;
    }

    printf("\n--- Tarefas Concluidas (%d) ---\n", p->tamanho);
    No *atual = p->topo;
    int numero = 1;
    while (atual != NULL) {
        printf("%d. %s\n", numero++, atual->descricao);
        atual = atual->proximo;
    }
}

void concluir_tarefa(Fila *f, Pilha *p) {
    if (f->inicio == NULL) {
        printf("Nenhuma tarefa pendente para concluir.\n");
        return;
    }

    No *tarefa = fila_desenfileirar(f);
    printf("Tarefa concluida: \"%s\"\n", tarefa->descricao);
    pilha_empilhar(p, tarefa->descricao);
    free(tarefa);
}

void desfazer_conclusao(Fila *f, Pilha *p) {
    if (p->topo == NULL) {
        printf("Nenhuma conclusao para desfazer.\n");
        return;
    }

    No *tarefa = pilha_desempilhar(p);
    printf("Conclusao desfeita: \"%s\" voltou para pendentes.\n", tarefa->descricao);

    /* Devolve ao inicio da fila para manter a ordem original */
    No *novo = (No *)malloc(sizeof(No));
    strncpy(novo->descricao, tarefa->descricao, MAX_DESCRICAO - 1);
    novo->descricao[MAX_DESCRICAO - 1] = '\0';
    novo->proximo = f->inicio;
    f->inicio = novo;
    if (f->fim == NULL) f->fim = novo;
    f->tamanho++;

    free(tarefa);
}

/* ============================================================
   MAIN
   ============================================================ */

int main(void) {
    Fila pendentes;
    Pilha concluidas;

    fila_inicializar(&pendentes);
    pilha_inicializar(&concluidas);

    /* Carrega dados salvos anteriormente */
    carregar_arquivo(&pendentes, &concluidas);

    int opcao;
    do {
        printf("\n=== Gerenciador de Tarefas ===\n");
        printf("Pendentes: %d | Concluidas: %d\n", pendentes.tamanho, concluidas.tamanho);
        printf("------------------------------\n");
        printf("1 - Adicionar tarefa\n");
        printf("2 - Listar tarefas pendentes\n");
        printf("3 - Listar tarefas concluidas\n");
        printf("4 - Concluir proxima tarefa\n");
        printf("5 - Desfazer ultima conclusao\n");
        printf("6 - Salvar e Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar(); /* limpa o '\n' que sobra no buffer apos scanf */

        printf("\n");

        switch (opcao) {
            case 1: adicionar_tarefa(&pendentes);            break;
            case 2: listar_pendentes(&pendentes);            break;
            case 3: listar_concluidas(&concluidas);          break;
            case 4: concluir_tarefa(&pendentes, &concluidas); break;
            case 5: desfazer_conclusao(&pendentes, &concluidas); break;
            case 6: salvar_arquivo(&pendentes, &concluidas); break;
            default: printf("Opcao invalida. Tente novamente.\n");
        }

    } while (opcao != 6);

    /* Libera memoria antes de encerrar */
    No *atual;
    while ((atual = fila_desenfileirar(&pendentes)) != NULL) free(atual);
    while ((atual = pilha_desempilhar(&concluidas)) != NULL) free(atual);

    printf("Ate logo!\n");
    return 0;
}