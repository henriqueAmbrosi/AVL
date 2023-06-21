#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct a {
    char value[50];
    int ocor;
    struct a *esq, *dir, *pai;
    // Valor que cresce conforme se aproxima da raiz (Funciona quase de forma inversa ao nível da árvore sendo um em todas as folhas)
    int altura;
};

typedef struct a Avl;


// Retorna a altura ou zero caso o nodo esteja nulo
int getAltura(Avl* node) {
    if (node == NULL)
        return 0;
    return node->altura;
}

// Calcula o fator de balanceamento de um nodo
int getFatorBalanceamento(Avl* node) {
   if (node == NULL)
        return 0;
    return getAltura(node->esq) - getAltura(node->dir);
}

// Inicializa ou reseta o header
void init(Avl* letra[]){
    int i;
    for(i = 0; i < 26; i++){
        letra[i] = NULL;
    }
}

// Rotação para a esquerda
// A rotação principal deve ser 0 se for somente para corrigir o sinal
// E 1 caso contrário
void rpe(Avl **nodo, Avl* letra[]){
    Avl* no = *nodo;
    Avl* dir = no->dir;

    // Rotaciona
    no->dir = dir->esq;
    if(dir->esq != NULL){
        // Atualiza pai do antigo nodo da esquerda da nova raiz para a raiz antiga
        dir->esq->pai = no;
    }
    dir->esq = (*nodo);

    // Atualiza os pais
    printf("teste: %s\n", (*nodo)->value);
    dir->pai = no->pai;
    printf("teste: %s\n", (*nodo)->value);
    no->pai = dir;

    // Atualiza alturas
    no->altura = 1 + (getAltura(no->dir) > getAltura(no->esq) ? getAltura(no->dir) : getAltura(no->esq));
    dir->altura = 1 + (no->altura > getAltura(dir->dir) ? no->altura : getAltura(dir->dir));

     // Atualiza a referência do vô
    if(dir->pai != NULL){
        if(strcmp(dir->pai->value, dir->value) < 0){
            dir->pai->dir = dir;
        }else{
            dir->pai->esq = dir;
        }
    }else{
        // Atualiza o vetor de letras
        letra[dir->value[0] - 'a'] = dir;
    }
}

// Rotação para a direita
// A rotação principal deve ser 0 se for somente para corrigir o sinal
// E 1 caso contrário
void rpd(Avl **nodo, Avl* letra[]){
    Avl* no = *nodo;
    Avl* esq = no->esq;

    // Rotaciona
    no->esq = esq->dir;
    if(esq->dir != NULL){
        // Atualiza pai do antigo nodo da direita da nova raiz para a raiz antiga
        esq->dir->pai = no;
    }
    esq->dir = no;

    // Atualiza os pais
    esq->pai = no->esq;
    no->pai = esq;

    // Atualiza alturas
    no->altura = 1 + (getAltura(no->esq) > getAltura(no->dir) ? getAltura(no->esq) : getAltura(no->dir));
    esq->altura = 1 + (no->altura > getAltura(esq->esq) ? no->altura : getAltura(esq->esq));

     // Atualiza a referência do vô
    if(esq->pai != NULL){
        if(strcmp(esq->pai->value, esq->value) < 0){
            esq->pai->dir = esq;
        }else{
            esq->pai->esq = esq;
        }
    }else{
        // Atualiza o vetor de letras
        letra[esq->value[0] - 'a'] = esq;
    }
}

// Determina a rotação necessária para balancear a árvore
void rebalancearArvore(Avl **pai, Avl* letra[]){
    int fbPai = getFatorBalanceamento(*pai);
    int fbFilho;
    if(fbPai == 2){
        // Braço da direita mais pesado: Rotação para a esquerda

        fbFilho = getFatorBalanceamento((*pai)->dir);
        if(fbFilho < 0){

            // Sinais opostos: Rotação dupla
            rpd(&(*pai)->dir, letra);
            rpe(&(*pai)->pai, letra);
        }else{
            // Sinais iguais: Rotação simples
            rpe(pai, letra);
        }
    }else{
        // Braço da esquerda mais pesado: Rotação para a direita

        fbFilho = getFatorBalanceamento((*pai)->dir);
        if(fbFilho > 0){
            // Sinais opostos: Rotação dupla
            rpe(&(*pai)->esq, letra);
            rpd(&(*pai)->pai, letra);
        }else{
            // Sinais iguais: Rotação simples
            rpd(pai, letra);
        }
    }
}

// Calcula altura dos pais do nodo recursivamente
void calcularAlturaPai(Avl **pai, Avl* letra[]){

    // Atualiza altura do nodo utilizando a altura do filho mais alto
    if(getAltura((*pai)->esq) > getAltura((*pai)->dir)){
        // Filho mais alto na esquerda
        (*pai)->altura = getAltura((*pai)->esq) + 1;
    }else{
        // Filho mais alto na direita
        (*pai)->altura = getAltura((*pai)->dir) + 1;
    }

    // Caso encontre um Fator de Balanceamento igual a 2 ou -2, rebalanceia
    int fb = getFatorBalanceamento(*pai);
    if(fb == 2 || fb == -2)
        rebalancearArvore(pai, letra);

    // Chama recursivamente
    if((*pai)->pai != NULL)
        calcularAlturaPai(&((*pai)->pai), letra);

}

void recalcularAltura(Avl **nodo, Avl* letra[]){
    printf("%s\n", (*nodo)->value);
    calcularAlturaPai(&((*nodo)->pai), letra);
}

// Cria e insere o nodo na árvore
void inserirNodo(Avl **pai, char* val, Avl* letra[]){
    Avl *novo = (Avl*) malloc(sizeof(Avl));
    strcpy(novo->value, val);
    novo->ocor = 1;
    novo->altura = 1;
    novo->esq = NULL;
    novo->dir = NULL;
    novo->pai = *pai;
    if(*pai != NULL && strcmp(val, (*pai)->value) < 0){
        (*pai)->esq = novo;
    }else if(*pai != NULL){
        (*pai)->dir = novo;
    }

    recalcularAltura(&novo, letra);
}

// Busca o local para inserir o nodo e insere na árvore com a função de inserirNodo
void inserirValorNaArvore(char* val, Avl** raiz, Avl* letra[]){

    // Primeira inserção na árvore
    if(*raiz == NULL){
        *raiz = (Avl*) malloc(sizeof(Avl));
        strcpy((*raiz)->value, val);
        (*raiz)->ocor = 1;
        (*raiz)->altura = 1;
        (*raiz)->esq = NULL;
        (*raiz)->dir = NULL;
        (*raiz)->pai = NULL;
    }else{
        // Segue busca para a esquerda
        if(strcmp(val, (*raiz)->value) < 0){
            if((*raiz)->esq == NULL){
                inserirNodo(raiz, val, letra);
            }else{
                inserirValorNaArvore(val, &((*raiz)->esq), letra);
            }
        // Segue busca para a direita
        }else if(strcmp(val, (*raiz)->value) > 0){
            if((*raiz)->dir == NULL){
                inserirNodo(raiz, val, letra);
            }else{
                inserirValorNaArvore(val, &((*raiz)->dir), letra);
            }
        // Já está na árvore
        }else{
            (*raiz)->ocor += 1;
        }
    }
}

void printPreFix(Avl* raiz){
    if( raiz == NULL )
        return;

    printf("%s - %d - %d\n",raiz->value, raiz->ocor, raiz->altura);
    printPreFix( raiz->esq );
    printPreFix( raiz->dir );
}

void removeComUmFilho(Avl **raiz){
    if((*raiz)->pai != NULL){
        // Caso tenha pai
        if((*raiz)->pai->value > (*raiz)->value){
            if((*raiz)->esq != NULL){
                (*raiz)->esq->pai = (*raiz)->pai;
                (*raiz)->pai->esq = (*raiz)->esq;
            }else{
                (*raiz)->dir->pai = (*raiz)->pai;
                (*raiz)->pai->esq = (*raiz)->dir;
            }

        }else{
            if((*raiz)->esq != NULL){
                (*raiz)->esq->pai = (*raiz)->pai;
                (*raiz)->pai->dir = (*raiz)->esq;
            }else{
                (*raiz)->dir->pai = (*raiz)->pai;
                (*raiz)->pai->dir = (*raiz)->dir;
            }
        }
    }else{
        // Caso não tenha pai (chegou na raiz)
        if((*raiz)->esq != NULL){
            (*raiz)->esq->pai = NULL;
            *raiz = (*raiz)->esq;
        }else{
            (*raiz)->dir->pai = NULL;
            *raiz = (*raiz)->dir;
        }
    }
}

void removeFolha(Avl **raiz){
    if((*raiz)->pai != NULL){
        if((*raiz)->pai->esq != NULL && (*raiz)->pai->esq->value == (*raiz)->value){
            (*raiz)->pai->esq = NULL;
        }else{
            (*raiz)->pai->dir = NULL;
        }
    }
    free(*raiz);
}

void removeComDoisFilhos(Avl **raiz){
    Avl *aux = *raiz;
    aux = aux->dir;
    while(aux->esq != NULL){
        aux = aux->esq;
    }

    char *value = aux->value;
    if(aux->dir == NULL && aux->esq == NULL){
        removeFolha(&aux);
    }else{
        removeComUmFilho(&aux);
    }

    strcpy((*raiz)->value, value);
    free(aux);
}


void removerNodo(int val, Avl **raiz){
    if(*raiz != NULL){
        if((*raiz)->value == val){
            if((*raiz)->dir == NULL && (*raiz)->esq == NULL){
                removeFolha(raiz);
            }else if((*raiz)->dir == NULL || (*raiz)->esq == NULL){
                removeComUmFilho(raiz);
            }else{
                removeComDoisFilhos(raiz);
            }
        }else if(val < (*raiz)->value){
            removerNodo(val, &((*raiz)->esq));
        }else{
            removerNodo(val, &((*raiz)->dir));
        }
    }
}

/* Quebra uma frase em palavras e adiciona elas na árvore
    - utiliza caracteres de quebra para separar as palavras
    - caracter de quebra é considerado qualquer caracter que não seja uma letra maiuscula ou minuscula
*/
void getJustWords(char *text, Avl* letra[]){
    int i, j = 0;
    char str[50], c;

    for(i = 0; text[i] != '\0'; i++){

        if(j > 50){
            // Caso uma palavra tenha mais de 50 caracteres
            printf("ERRO: Palavra ultrapassou limite máximo de 50 caracteres.");
        }

        if((text[i] >= 'a' && text[i] <= 'z') || (text[i] >= 'A' && text[i] <= 'Z')){
            // Encontrou uma letra, então só adiciona na palavra
            // A palavra deve ser armazenada com letras minúsculas
            c = tolower(text[i]);
            str[j] = c;
            j++;
        } else {
            // Encontrou um caracter de quebra, então adiciona a palavra à lista

            // Evita salvar palavras sem conteúdo (previne casos de , seguidos de espaço por exemplo)
            if(j != 0){
                str[j] = '\0';
                c = tolower(str[0]);
                inserirValorNaArvore(str, &(letra[c - 'a']), letra);
                j = 0;
            }
        }
    }

    // Adiciona a última palavra caso não encontre caracteres de quebra como o ponto no final do texto
    if(j != 0){
        str[j] = '\0';
        c = tolower(str[0]);
        inserirValorNaArvore(str, &(letra[c - 'a']), letra);
        j = 0;
    }
}


// Exibe o menu
void showMenu(){
    printf("Menu:\n");
    printf("0. Sair\n");
    printf("1. Inserir mais texto\n");
    printf("2. Limpar textos\n");
    printf("3. Buscar palavra\n");
    printf("4. Remover palavra\n");
    printf("5. Mostrar total de palavras\n");
    printf("6. Mostrar total de palavras diferentes\n");
    printf("7. Exibir lista de palavras em ordem alfabetica\n");
    printf("8. Pesquisar palavras por letra\n");
    printf("9. Exibir lista de palavras em ordem de ocorrencias\n");
    printf("10. Filtrar palavras por numero de ocorrencias\n");
    printf("\n");
}

void mostrarConteudo( Avl* letra[]){
    int i;
    for(i = 0; i < 26; i++){
            if(letra[i] != NULL){
                printf("%c:\n", 'A' + i);
                printPreFix(letra[i]);
                printf("\n");
            }
        }
}

void main(){
    // Inicia o cabeçalho
    Avl h[26];
    init(&h);

    // Obtém a frase
    char text[5000];
    printf("Informe o texto:\n");
    //gets(text);
    strcpy(text, "ag ad am af aa ab ae ay ap ac az");

    // Declara variáveis de uso geral
    int opt, subopt;
    Avl *aux;

    // Processa frase removendo sinais de pontuação e alocando na lista encadeada
    getJustWords(text, h);

    mostrarConteudo(h);
}




