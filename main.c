#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_HASH 101

// Estrutura para representar um item
typedef struct Item {
    char nome[50];
    int poder;
    struct Item* proximo;
} Item;

// Estrutura para representar um jogador
typedef struct Jogador {
    char nome[50];
    int saude;
    Item* inventario;
    struct Jogador* proximo;  // para lista circular
    struct Jogador* anterior;  // para lista circular
} Jogador;

// Estrutura para representar o estado do jogo
typedef struct EstadoJogo {
    Jogador* jogadores;  // lista circular de jogadores
    struct EstadoJogo* anterior;
} EstadoJogo;

// Estrutura para representar uma ação
typedef struct Acao {
    char descricao[100];
    char tipo[10];  // "atacar" ou "curar"
    char alvo[50];  // jogador alvo
    struct Acao* proximo;
} Acao;

// Estrutura para representar um nó da pilha
typedef struct NoPilha {
    EstadoJogo estado;
    struct NoPilha* proximo;
} NoPilha;

// Estrutura para representar um nó da fila
typedef struct NoFila {
    Acao acao;
    struct NoFila* proximo;
} NoFila;

// Estrutura para representar um nó da tabela de hash
typedef struct NoHash {
    char chave[50];
    Jogador* jogador;
    struct NoHash* proximo;
} NoHash;

// Tabela de hash
NoHash* tabelaHash[TAMANHO_HASH];

// Funções de Lista Circular
Jogador* criarJogador(char* nome, int saude) {
    Jogador* novoJogador = (Jogador*)malloc(sizeof(Jogador));
    strcpy(novoJogador->nome, nome);
    novoJogador->saude = saude;
    novoJogador->inventario = NULL;
    novoJogador->proximo = novoJogador->anterior = novoJogador;
    return novoJogador;
}

void adicionarJogador(Jogador** cabeca, Jogador* novoJogador) {
    if (*cabeca == NULL) {
        *cabeca = novoJogador;
    } else {
        Jogador* cauda = (*cabeca)->anterior;
        cauda->proximo = novoJogador;
        novoJogador->anterior = cauda;
        novoJogador->proximo = *cabeca;
        (*cabeca)->anterior = novoJogador;
    }
}

void removerJogador(Jogador** cabeca, char* nome) {
    Jogador* atual = *cabeca;
    if (!atual) return;
    do {
        if (strcmp(atual->nome, nome) == 0) {
            if (atual->proximo == atual) {
                free(atual);
                *cabeca = NULL;
                return;
            } else {
                atual->anterior->proximo = atual->proximo;
                atual->proximo->anterior = atual->anterior;
                if (*cabeca == atual) *cabeca = atual->proximo;
                free(atual);
                return;
            }
        }
        atual = atual->proximo;
    } while (atual != *cabeca);
}

void imprimirJogadores(Jogador* cabeca) {
    Jogador* atual = cabeca;
    if (cabeca == NULL) return;
    do {
        printf("Jogador: %s, Saúde: %d\n", atual->nome, atual->saude);
        atual = atual->proximo;
    } while (atual != cabeca);
}

// Funções de Lista Duplamente Encadeada (Inventário)
void adicionarItem(Item** cabeca, char* nome) {
    Item* novoItem = (Item*)malloc(sizeof(Item));
    strcpy(novoItem->nome, nome);
    novoItem->proximo = *cabeca;
    *cabeca = novoItem;
}

void removerItem(Item** cabeca, char* nome) {
    Item* atual = *cabeca;
    Item* anterior = NULL;
    while (atual != NULL && strcmp(atual->nome, nome) != 0) {
        anterior = atual;
        atual = atual->proximo;
    }
    if (atual == NULL) return;
    if (anterior == NULL) *cabeca = atual->proximo;
    else anterior->proximo = atual->proximo;
    free(atual);
}

void imprimirInventario(Item* cabeca) {
    Item* atual = cabeca;
    while (atual != NULL) {
        printf("Item: %s\n", atual->nome);
        atual = atual->proximo;
    }
}

// Funções de Lista Simplesmente Encadeada (Ações)
void adicionarAcao(Acao** cabeca, char* descricao, char* tipo, char* alvo) {
    Acao* novaAcao = (Acao*)malloc(sizeof(Acao));
    strcpy(novaAcao->descricao, descricao);
    strcpy(novaAcao->tipo, tipo);
    strcpy(novaAcao->alvo, alvo);
    novaAcao->proximo = *cabeca;
    *cabeca = novaAcao;
}

void removerAcao(Acao** cabeca) {
    if (*cabeca == NULL) return;
    Acao* temp = *cabeca;
    *cabeca = (*cabeca)->proximo;
    free(temp);
}

void imprimirAcoes(Acao* cabeca) {
    Acao* atual = cabeca;
    int indice = 0;
    while (atual != NULL) {
        printf("%d. Ação: %s, Tipo: %s, Alvo: %s\n", indice, atual->descricao, atual->tipo, atual->alvo);
        atual = atual->proximo;
        indice++;
    }
}

Acao* obterAcao(Acao* cabeca, int indice) {
    Acao* atual = cabeca;
    int i = 0;
    while (atual != NULL && i < indice) {
        atual = atual->proximo;
        i++;
    }
    return atual;
}

// Funções de Pilha (Histórico de Estados do Jogo)
void empilhar(NoPilha** topo, EstadoJogo estado) {
    NoPilha* novoNo = (NoPilha*)malloc(sizeof(NoPilha));
    novoNo->estado = estado;
    novoNo->proximo = *topo;
    *topo = novoNo;
}

EstadoJogo desempilhar(NoPilha** topo) {
    if (*topo == NULL) exit(1); // Pilha vazia
    NoPilha* temp = *topo;
    EstadoJogo estadoDesempilhado = temp->estado;
    *topo = (*topo)->proximo;
    free(temp);
    return estadoDesempilhado;
}

void imprimirPilha(NoPilha* topo) {
    NoPilha* atual = topo;
    while (atual != NULL) {
        printf("Estado do Jogo:\n");
        imprimirJogadores(atual->estado.jogadores);
        atual = atual->proximo;
    }
}

// Funções de Fila (Fila de Eventos)
void enfileirar(NoFila** frente, NoFila** traseira, Acao acao) {
    NoFila* novoNo = (NoFila*)malloc(sizeof(NoFila));
    novoNo->acao = acao;
    novoNo->proximo = NULL;
    if (*traseira == NULL) {
        *frente = *traseira = novoNo;
        return;
    }
    (*traseira)->proximo = novoNo;
    *traseira = novoNo;
}

Acao desenfileirar(NoFila** frente, NoFila** traseira) {
    if (*frente == NULL) exit(1); // Fila vazia
    NoFila* temp = *frente;
    Acao acaoDesenfileirada = temp->acao;
    *frente = (*frente)->proximo;
    if (*frente == NULL) *traseira = NULL;
    free(temp);
    return acaoDesenfileirada;
}

void imprimirFila(NoFila* frente) {
    NoFila* atual = frente;
    while (atual != NULL) {
        printf("Ação na Fila: %s, Tipo: %s, Alvo: %s\n", atual->acao.descricao, atual->acao.tipo, atual->acao.alvo);
        atual = atual->proximo;
    }
}

// Funções de Hashing
unsigned int hash(char* chave) {
    unsigned int hash = 0;
    while (*chave) {
        hash = (hash << 5) + *chave++;
    }
    return hash % TAMANHO_HASH;
}

void inserirHash(char* chave, Jogador* jogador) {
    unsigned int indice = hash(chave);
    NoHash* novoNo = (NoHash*)malloc(sizeof(NoHash));
    strcpy(novoNo->chave, chave);
    novoNo->jogador = jogador;
    novoNo->proximo = tabelaHash[indice];
    tabelaHash[indice] = novoNo;
}

Jogador* buscarHash(char* chave) {
    unsigned int indice = hash(chave);
    NoHash* atual = tabelaHash[indice];
    while (atual != NULL && strcmp(atual->chave, chave) != 0) {
        atual = atual->proximo;
    }
    return atual ? atual->jogador : NULL;
}

// Função para salvar o estado do jogo
void salvarEstadoJogo(NoPilha** pilhaEstadoJogo, Jogador* jogadores) {
    EstadoJogo estadoJogo;
    
    // Copiar profundamente os jogadores
    if (jogadores) {
        Jogador* atual = jogadores;
        Jogador* novosJogadores = NULL;
        do {
            Jogador* novoJogador = criarJogador(atual->nome, atual->saude);
            adicionarJogador(&novosJogadores, novoJogador);
            
            // Copiar profundamente o inventário
            Item* itemAtual = atual->inventario;
            while (itemAtual) {
                adicionarItem(&(novoJogador->inventario), itemAtual->nome);
                itemAtual = itemAtual->proximo;
            }
            atual = atual->proximo;
        } while (atual != jogadores);
        estadoJogo.jogadores = novosJogadores;
    } else {
        estadoJogo.jogadores = NULL;
    }
    
    empilhar(pilhaEstadoJogo, estadoJogo);
}

// Função main
int main() {
    // Inicializa a tabela hash
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabelaHash[i] = NULL;
    }

    Jogador* jogadores = NULL;
    NoPilha* pilhaEstadoJogo = NULL;
    NoFila* filaEventosFrente = NULL;
    NoFila* filaEventosTraseira = NULL;
    Acao* acoes = NULL;

    int escolha;
    char nome[50];
    int saude;
    char nomeItem[50];
    char descricaoAcao[100];
    char tipoAcao[10];
    char alvoAcao[50];
    int indiceAcao;

    while (1) {
        printf("\nMenu:\n");
        printf("1. Adicionar Jogador\n");
        printf("2. Remover Jogador\n");
        printf("3. Adicionar Item ao Inventário\n");
        printf("4. Remover Item do Inventário\n");
        printf("5. Adicionar Ação\n");
        printf("6. Remover Ação\n");
        printf("7. Executar Ação\n");
        printf("8. Visualizar Jogadores\n");
        printf("9. Visualizar Inventário de um Jogador\n");
        printf("10. Visualizar Ações\n");
        printf("11. Visualizar Fila de Eventos\n");
        printf("12. Visualizar Histórico de Estados do Jogo\n");
        printf("13. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &escolha);

        switch (escolha) {
            case 1:
                printf("Nome do Jogador: ");
                scanf("%s", nome);
                printf("Saúde do Jogador: ");
                scanf("%d", &saude);
                Jogador* novoJogador = criarJogador(nome, saude);
                adicionarJogador(&jogadores, novoJogador);
                inserirHash(nome, novoJogador);
                salvarEstadoJogo(&pilhaEstadoJogo, jogadores);
                break;
            case 2:
                printf("Nome do Jogador a ser Removido: ");
                scanf("%s", nome);
                removerJogador(&jogadores, nome);
                salvarEstadoJogo(&pilhaEstadoJogo, jogadores);
                break;
            case 3:
                printf("Nome do Jogador: ");
                scanf("%s", nome);
                printf("Nome do Item: ");
                scanf("%s", nomeItem);
                Jogador* jogador = buscarHash(nome);
                if (jogador) {
                    adicionarItem(&(jogador->inventario), nomeItem);
                    salvarEstadoJogo(&pilhaEstadoJogo, jogadores);
                } else {
                    printf("Jogador não encontrado.\n");
                }
                break;
            case 4:
                printf("Nome do Jogador: ");
                scanf("%s", nome);
                printf("Nome do Item a ser Removido: ");
                scanf("%s", nomeItem);
                jogador = buscarHash(nome);
                if (jogador) {
                    removerItem(&(jogador->inventario), nomeItem);
                    salvarEstadoJogo(&pilhaEstadoJogo, jogadores);
                } else {
                    printf("Jogador não encontrado.\n");
                }
                break;
            case 5:
                printf("Descrição da Ação: ");
                scanf("%s", descricaoAcao);
                printf("Tipo da Ação (atacar/curar): ");
                scanf("%s", tipoAcao);
                printf("Jogador Alvo da Ação: ");
                scanf("%s", alvoAcao);
                adicionarAcao(&acoes, descricaoAcao, tipoAcao, alvoAcao);
                enfileirar(&filaEventosFrente, &filaEventosTraseira, *acoes);
                salvarEstadoJogo(&pilhaEstadoJogo, jogadores);
                break;
            case 6:
                removerAcao(&acoes);
                salvarEstadoJogo(&pilhaEstadoJogo, jogadores);
                break;
            case 7:
                if (filaEventosFrente) {
                    printf("Ações disponíveis:\n");
                    imprimirAcoes(acoes);
                    printf("Escolha o índice da ação para executar: ");
                    scanf("%d", &indiceAcao);
                    Acao* acaoParaExecutar = obterAcao(acoes, indiceAcao);
                    if (acaoParaExecutar) {
                        printf("Executando Ação: %s, Tipo: %s, Alvo: %s\n", acaoParaExecutar->descricao, acaoParaExecutar->tipo, acaoParaExecutar->alvo);
                        Jogador* jogadorAlvo = buscarHash(acaoParaExecutar->alvo);
                        if (jogadorAlvo) {
                            if (strcmp(acaoParaExecutar->tipo, "atacar") == 0) {
                                jogadorAlvo->saude -= 10; // exemplo: diminui 10 de saúde no ataque
                            } else if (strcmp(acaoParaExecutar->tipo, "curar") == 0) {
                                jogadorAlvo->saude += 10; // exemplo: aumenta 10 de saúde na cura
                            }
                            printf("Nova Saúde de %s: %d\n", jogadorAlvo->nome, jogadorAlvo->saude);
                            salvarEstadoJogo(&pilhaEstadoJogo, jogadores);
                        } else {
                            printf("Jogador alvo não encontrado.\n");
                        }
                        removerAcao(&acoes);
                    } else {
                        printf("Índice de ação inválido.\n");
                    }
                } else {
                    printf("Nenhuma ação disponível para executar.\n");
                }
                break;
            case 8:
                imprimirJogadores(jogadores);
                break;
            case 9:
                printf("Nome do Jogador: ");
                scanf("%s", nome);
                jogador = buscarHash(nome);
                if (jogador) {
                    imprimirInventario(jogador->inventario);
                } else {
                    printf("Jogador não encontrado.\n");
                }
                break;
            case 10:
                imprimirAcoes(acoes);
                break;
            case 11:
                imprimirFila(filaEventosFrente);
                break;
            case 12:
                imprimirPilha(pilhaEstadoJogo);
                break;
            case 13:
                exit(0);
                break;
            default:
                printf("Opção inválida.\n");
        }
    }

    return 0;
}
