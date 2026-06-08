# Gerenciador de Tarefas CLI

Projeto final da disciplina de Estruturas de Dados — Sistema de linha de comando (CLI) escrito em C puro (C11).

## Descrição do problema

No dia a dia é comum acumular tarefas e perder o controle sobre o que já foi feito ou não. Este sistema permite ao usuário adicionar tarefas, visualizá-las em ordem de chegada, concluir a próxima da fila e desfazer a última conclusão caso tenha sido um engano. Todos os dados são salvos em disco e recuperados na próxima execução.

## Como compilar e executar

**Requisitos:** GCC instalado (MinGW-w64 no Windows).

```bash
gcc main.c -o tarefas
./tarefas.exe   # Windows
./tarefas       # Linux/Mac
```

## Funcionalidades

| Opção | Descrição |
|-------|-----------|
| 1 | Adicionar nova tarefa |
| 2 | Listar tarefas pendentes |
| 3 | Listar tarefas concluídas |
| 4 | Concluir a próxima tarefa da fila |
| 5 | Desfazer a última conclusão |
| 6 | Salvar e sair |

## Estruturas de dados utilizadas

### Fila (tarefas pendentes)
A fila armazena as tarefas que ainda não foram concluídas. A estrutura segue o princípio **FIFO** (*First In, First Out*) — a tarefa adicionada primeiro é a primeira a ser concluída, o que reflete a ordem natural de uma lista de afazeres. A fila é implementada com nós encadeados e ponteiros para o início e o fim, permitindo inserção em O(1) no final e remoção em O(1) no início.

### Pilha (histórico de conclusões)
A pilha armazena as tarefas já concluídas e serve como histórico para a operação de desfazer. A estrutura segue o princípio **LIFO** (*Last In, First Out*) — a última tarefa concluída é a primeira a ser desfeita, que é exatamente o comportamento esperado de um "Ctrl+Z". A pilha é implementada com nós encadeados e um único ponteiro de topo.

Ambas as estruturas compartilham o mesmo tipo de nó (`struct No`) e foram implementadas do zero, sem uso de bibliotecas externas.

## Formato do arquivo de persistência

O sistema utiliza o arquivo `tarefas.csv` no mesmo diretório do executável.

**Formato:** `status,descricao`

```
pendente,Estudar para a prova
pendente,Fazer exercicios de C
concluida,Comprar material escolar
```

**Justificativa:** O formato CSV foi escolhido por ser simples de ler e escrever com as funções padrão do C (`fprintf`, `fgets`, `strchr`), fácil de inspecionar manualmente em qualquer editor de texto, e suficiente para os dados do sistema (apenas status e descrição).

**Casos de borda tratados:**
- Arquivo inexistente na primeira execução: o programa inicia normalmente com listas vazias.
- Arquivo vazio: o programa inicia normalmente sem erros.
- Linhas malformadas (sem vírgula): são ignoradas silenciosamente.

## Estrutura do projeto

```
ProjetoC/
└── main.c       # Todo o código-fonte em um único arquivo
└── README.md    # Este arquivo
```

## Limitações conhecidas

- A descrição da tarefa não pode conter vírgulas, pois a vírgula é usada como separador no arquivo CSV.
- Não há limite de tarefas, mas a descrição de cada tarefa é limitada a 199 caracteres.
- Não há ordenação por prioridade — as tarefas seguem estritamente a ordem de inserção.
- O arquivo é salvo apenas ao escolher a opção "Salvar e Sair". Fechar o terminal sem usar essa opção resultará em perda dos dados da sessão.