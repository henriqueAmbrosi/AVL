#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct a
{
    char value[50];
    int ocor;
    struct a *esq, *dir, *pai;
    // Valor que cresce conforme se aproxima da raiz (Funciona quase de forma inversa ao nível da árvore sendo um em todas as folhas)
    int altura;
};

typedef struct a Avl;

// Inicializa ou reseta o header
void init(Avl *letra[])
{
    int i;
    for (i = 0; i < 26; i++)
    {
        letra[i] = NULL;
    }
}

// Retorna a altura ou zero caso o nodo esteja nulo
int getAltura(Avl *node)
{
    if (node == NULL)
        return 0;
    return node->altura;
}

// Calcula o fator de balanceamento de um nodo
int getFatorBalanceamento(Avl *node)
{
    if (node == NULL)
        return 0;
    return getAltura(node->dir) - getAltura(node->esq);
}

// Rotação para a esquerda
void rpe(Avl **nodo, Avl *letra[])
{
    Avl *no = *nodo;
    Avl *dir = no->dir;

    // Rotaciona
    no->dir = dir->esq;
    if (dir->esq != NULL)
    {
        // Atualiza pai do antigo nodo da esquerda da nova raiz para a raiz antiga
        dir->esq->pai = no;
    }
    dir->esq = (*nodo);

    // Atualiza os pais
    dir->pai = no->pai;
    no->pai = dir;

    // Atualiza alturas
    no->altura = 1 + (getAltura(no->dir) > getAltura(no->esq) ? getAltura(no->dir) : getAltura(no->esq));
    dir->altura = 1 + (no->altura > getAltura(dir->dir) ? no->altura : getAltura(dir->dir));

    // Atualiza a referência do vô
    if (dir->pai != NULL)
    {
        if (strcmp(dir->pai->value, dir->value) < 0)
        {
            dir->pai->dir = dir;
        }
        else
        {
            dir->pai->esq = dir;
        }
    }
    else
    {
        // Atualiza o vetor de letras
        letra[dir->value[0] - 'a'] = dir;
    }
}

// Rotação para a direita
void rpd(Avl **nodo, Avl *letra[])
{
    Avl *no = *nodo;
    Avl *esq = no->esq;

    // Rotaciona
    no->esq = esq->dir;
    if (esq->dir != NULL)
    {
        // Atualiza pai do antigo nodo da direita da nova raiz para a raiz antiga
        esq->dir->pai = no;
    }
    esq->dir = no;

    // Atualiza os pais
    esq->pai = no->pai;
    no->pai = esq;

    // Atualiza alturas
    no->altura = 1 + (getAltura(no->esq) > getAltura(no->dir) ? getAltura(no->esq) : getAltura(no->dir));
    esq->altura = 1 + (no->altura > getAltura(esq->esq) ? no->altura : getAltura(esq->esq));

    // Atualiza a referência do vô
    if (esq->pai != NULL)
    {
        if (strcmp(esq->pai->value, esq->value) < 0)
        {
            esq->pai->dir = esq;
        }
        else
        {
            esq->pai->esq = esq;
        }
    }
    else
    {
        // Atualiza o vetor de letras
        letra[esq->value[0] - 'a'] = esq;
    }
}

// Determina a rotação necessária para balancear a árvore
void rebalancearArvore(Avl **pai, Avl *letra[])
{
    int fbPai = getFatorBalanceamento(*pai);
    int fbFilho;

    if (fbPai == 2)
    {
        // Braço da direita mais pesado: Rotação para a esquerda

        fbFilho = getFatorBalanceamento((*pai)->dir);
        if (fbFilho < 0)
        {
            // Sinais opostos: Rotação dupla
            rpd(&(*pai)->dir, letra);
            rpe(&(*pai)->pai, letra);
        }
        else
        {
            // Sinais iguais: Rotação simples
            rpe(pai, letra);
        }
    }
    else
    {
        // Braço da esquerda mais pesado: Rotação para a direita

        fbFilho = getFatorBalanceamento((*pai)->esq);
        if (fbFilho > 0)
        {
            // Sinais opostos: Rotação dupla
            rpe(&(*pai)->esq, letra);
            rpd(&(*pai)->pai, letra);
        }
        else
        {
            // Sinais iguais: Rotação simples
            rpd(pai, letra);
        }
    }
}

// Calcula altura dos pais do nodo recursivamente
void calcularAlturaPai(Avl **pai, Avl *letra[])
{
    // Atualiza altura do nodo utilizando a altura do filho mais alto
    if (getAltura((*pai)->esq) > getAltura((*pai)->dir))
    {
        // Filho mais alto na esquerda
        (*pai)->altura = getAltura((*pai)->esq) + 1;
    }
    else
    {
        // Filho mais alto na direita
        (*pai)->altura = getAltura((*pai)->dir) + 1;
    }

    // Caso encontre um Fator de Balanceamento igual a 2 ou -2, rebalanceia

    int fb = getFatorBalanceamento(*pai);
    if (fb == 2 || fb == -2)
        rebalancearArvore(pai, letra);

    // Chama recursivamente
    if ((*pai) != NULL && (*pai)->pai != NULL)
        calcularAlturaPai(&((*pai)->pai), letra);
}

void recalcularAltura(Avl **nodo, Avl *letra[])
{
    if (*nodo != NULL && (*nodo)->pai != NULL)
        calcularAlturaPai(&((*nodo)->pai), letra);
}

// Cria e insere o nodo na árvore
void inserirNodo(Avl **pai, char *val, Avl *letra[])
{
    Avl *novo = (Avl *) malloc(sizeof(Avl));
    strcpy(novo->value, val);
    novo->ocor = 1;
    novo->altura = 1;
    novo->esq = NULL;
    novo->dir = NULL;
    novo->pai = *pai;
    if (*pai != NULL && strcmp(val, (*pai)->value) < 0)
    {
        (*pai)->esq = novo;
    }
    else if (*pai != NULL)
    {
        (*pai)->dir = novo;
    }

    recalcularAltura(&novo, letra);
}

// Busca o local para inserir o nodo e insere na árvore com a função de inserirNodo
void inserirValorNaArvore(char *val, Avl **raiz, Avl *letra[])
{

    // Primeira inserção na árvore
    if (*raiz == NULL)
    {
        *raiz = (Avl *) malloc(sizeof(Avl));
        strcpy((*raiz)->value, val);
        (*raiz)->ocor = 1;
        (*raiz)->altura = 1;
        (*raiz)->esq = NULL;
        (*raiz)->dir = NULL;
        (*raiz)->pai = NULL;
    }
    else
    {
        // Segue busca para a esquerda
        if (strcmp(val, (*raiz)->value) < 0)
        {
            if ((*raiz)->esq == NULL)
            {
                inserirNodo(raiz, val, letra);
            }
            else
            {
                inserirValorNaArvore(val, &((*raiz)->esq), letra);
            }
            // Segue busca para a direita
        }
        else if (strcmp(val, (*raiz)->value) > 0)
        {
            if ((*raiz)->dir == NULL)
            {
                inserirNodo(raiz, val, letra);
            }
            else
            {
                inserirValorNaArvore(val, &((*raiz)->dir), letra);
            }
            // Já está na árvore
        }
        else
        {
            (*raiz)->ocor += 1;
        }
    }
}

/* Quebra uma frase em palavras e adiciona elas na árvore
    - utiliza caracteres de quebra para separar as palavras
    - caracter de quebra é considerado qualquer caracter que não seja uma letra maiuscula ou minuscula
*/
void getJustWords(char *text, Avl *letra[])
{
    int i, j = 0;
    char str[50], c;

    for (i = 0; text[i] != '\0'; i++)
    {

        if (j > 50)
        {
            // Caso uma palavra tenha mais de 50 caracteres
            printf("ERRO: Uma palavra ultrapassou limite máximo de 50 caracteres.");
            exit(1);
        }

        if ((text[i] >= 'a' && text[i] <= 'z') || (text[i] >= 'A' && text[i] <= 'Z'))
        {
            // Encontrou uma letra, então só adiciona na palavra
            // A palavra deve ser armazenada com letras minúsculas
            c = tolower(text[i]);
            str[j] = c;
            j++;
        }
        else
        {
            // Encontrou um caracter de quebra, então adiciona a palavra à árvore

            // Evita salvar palavras sem conteúdo (previne casos de , seguidos de espaço por exemplo)
            if (j != 0)
            {
                str[j] = '\0';
                c = tolower(str[0]);
                inserirValorNaArvore(str, &(letra[c - 'a']), letra);
                j = 0;
            }
        }
    }

    // Adiciona a última palavra caso não encontre caracteres de quebra como o ponto no final do texto
    if (j != 0)
    {
        str[j] = '\0';
        c = tolower(str[0]);
        inserirValorNaArvore(str, &(letra[c - 'a']), letra);
        j = 0;
    }
}

void removeComUmFilho(Avl **nodo, Avl *letterHashTable[])
{
    // Raiz da árvore
    Avl *raiz = letterHashTable[(*nodo)->value[0] - 'a'];

    if ((*nodo)->pai != NULL)
    {
        // Caso tenha pai

        Avl *aux = *nodo;

        // Nodo a remover está na esquerda do pai
        if (strcmp((*nodo)->pai->value, (*nodo)->value) > 0)
        {
            // Filho do nodo removido está à esquerda
            if ((*nodo)->esq != NULL)
            {
                (*nodo)->esq->pai = (*nodo)->pai;
                (*nodo)->pai->esq = (*nodo)->esq;
                recalcularAltura(&(aux->esq), &raiz);
            }
            // Filho do nodo removido está à direita
            else
            {
                (*nodo)->dir->pai = (*nodo)->pai;
                (*nodo)->pai->esq = (*nodo)->dir;
                recalcularAltura(&(aux->dir), &raiz);
            }
        }
        // Nodo a remover está na direita do pai
        else
        {
            // Filho do nodo removido está à esquerda
            if ((*nodo)->esq != NULL)
            {
                (*nodo)->esq->pai = (*nodo)->pai;
                (*nodo)->pai->dir = (*nodo)->esq;
                recalcularAltura(&(aux->esq), &raiz);
            }
            // Filho do nodo removido está à direita
            else
            {
                (*nodo)->dir->pai = (*nodo)->pai;
                (*nodo)->pai->dir = (*nodo)->dir;
                recalcularAltura(&(aux->dir), &raiz);
            }
        }
    }
    else
    {
        // Caso não tenha pai (chegou na raiz)
        if ((*nodo)->esq != NULL)
        {
            (*nodo)->esq->pai = NULL;
            *nodo = (*nodo)->esq;
        }
        else if ((*nodo)->dir != NULL) {
            (*nodo)->dir->pai = NULL;
            *nodo = (*nodo)->dir;
        }
		recalcularAltura(&(*nodo), &raiz);
        letterHashTable[(*nodo)->value[0] - 'a'] = *nodo;
    }
}

void removeFolha(Avl **nodo, Avl *letterHashTable[])
{
    // Raiz da árvore
    Avl *raiz = letterHashTable[(*nodo)->value[0] - 'a'];

    if ((*nodo)->pai != NULL)
    {
        // Não é raiz da árvore

        Avl *aux = *nodo;

        // Folha a remover está na esquerda
        if ((*nodo)->pai->esq != NULL && strcmp((*nodo)->pai->esq->value, (*nodo)->value) == 0)
        {
            (*nodo)->pai->esq = NULL;
        }
        // Folha a remover está na direita
        else
        {
            (*nodo)->pai->dir = NULL;
        }
        recalcularAltura(&aux, &raiz);
    }
    else
    {
        // Caso seja o único elemento da árvore remove no array de letras
        letterHashTable[(*nodo)->value[0] - 'a'] = NULL;
    }
    free(*nodo);
}

void removeComDoisFilhos(Avl **nodo, Avl *letterHashTable[])
{
    Avl *aux = *nodo;

    // Obtém o menor valor maior que o do nodo a ser removido
    aux = aux->dir;
    while (aux->esq != NULL)
    {
        aux = aux->esq;
    }

    // Copia os dados a serem transferidos
    char value[50];
    strcpy(value, aux->value);
    int ocor = aux->ocor;

    // Remove o nodo do menor valor maior que o do nodo a ser removido
    if (aux->dir == NULL && aux->esq == NULL)
    {
        removeFolha(&aux, letterHashTable);
    }
    else
    {
        removeComUmFilho(&aux, letterHashTable);
    }

    // Copia os valores do nodo removido para o nodo que deveria ser removido originalmente
    strcpy((*nodo)->value, value);
    (*nodo)->ocor = ocor;
}

void removerNodo(char *val, Avl **raiz, Avl *letterHashTable[])
{
    if (*raiz != NULL)
    {
        if (strcmp((*raiz)->value, val) == 0)
        {
            if ((*raiz)->dir == NULL && (*raiz)->esq == NULL)
            {
                removeFolha(raiz, letterHashTable);
            }
            else if ((*raiz)->dir == NULL || (*raiz)->esq == NULL)
            {
                removeComUmFilho(raiz, letterHashTable);
            }
            else
            {
                removeComDoisFilhos(raiz, letterHashTable);
            }
        }
        else if (strcmp(val, (*raiz)->value) < 0)
        {
            removerNodo(val, &((*raiz)->esq), letterHashTable);
        }
        else
        {
            removerNodo(val, &((*raiz)->dir), letterHashTable);
        }
    }
    else
    {
        printf("Valor informado não encontrado!\n");
    }
}

void removerValor(char val[], Avl *letterHashTable[])
{
    removerNodo(val, &letterHashTable[val[0] - 'a'], letterHashTable);
}

void consultaOcorrencias(char *val, Avl *root)
{
    if (root == NULL)
    {
        printf("A palavra %s não tem ocorrencias.\n", val);
        return;
    }

    if (strcmp(val, root->value) == 0)
    {
        printf("A palavra %s tem %d ocorrencias.\n", val, root->ocor);
    }
    else if (strcmp(val, root->value) > 0)
    {
        consultaOcorrencias(val, root->dir);
    }
    else
    {
        consultaOcorrencias(val, root->esq);
    }
}

int getTamanhoPorLetra(Avl *nodo)
{
    if (nodo != NULL)
    {
        return 1 + getTamanhoPorLetra(nodo->esq) + getTamanhoPorLetra(nodo->dir);
    }
    else
    {
        return 0;
    }
}

int getOcorrenciaPorLetra(Avl *nodo)
{
    if (nodo != NULL)
    {
        return nodo->ocor + getOcorrenciaPorLetra(nodo->esq) + getOcorrenciaPorLetra(nodo->dir);
    }
    else
    {
        return 0;
    }
}

int getTamanho(Avl *h[])
{
    int soma = 0, i;
    for (i = 0; i < 26; i++)
    {
        soma += getTamanhoPorLetra(h[i]);
    }
    return soma;
}

int getOcorrencias(Avl *h[])
{
    int soma = 0, i;
    for (i = 0; i < 26; i++)
    {
        soma += getOcorrenciaPorLetra(h[i]);
    }
    return soma;
}

// Ordem:
// - A-Z: 1
// - Z-A: 0
void exibirPalavrasPorLetra(Avl **nodo, int ordem)
{
    if ((*nodo) != NULL)
    {
        if (ordem)
        {
            exibirPalavrasPorLetra(&(*nodo)->esq, ordem);
            printf("%s - %d\n", (*nodo)->value, (*nodo)->ocor);
            exibirPalavrasPorLetra(&(*nodo)->dir, ordem);
        }
        else
        {
            exibirPalavrasPorLetra(&(*nodo)->dir, ordem);
            printf("%s - %d\n", (*nodo)->value, (*nodo)->ocor);
            exibirPalavrasPorLetra(&(*nodo)->esq, ordem);
        }
    }
}

// Exibe todas as palavras
//
// Ordem:
// - A-Z: 1
// - Z-A: 0
void exibirPalavras(Avl *h[], int ordem)
{
    int i;
    if (ordem)
    {
        // A-Z
        for (i = 0; i < 26; i++)
        {
            if (h[i] != NULL)
            {
                printf("%c:\n", 'A' + i);
                exibirPalavrasPorLetra(&h[i], ordem);
                printf("\n");
            }
        }
    }
    else
    {
        // Z-A
        for (i = 25; i >= 0; i--)
        {
            if (h[i] != NULL)
            {
                printf("%c:\n", 'A' + i);
                exibirPalavrasPorLetra(&h[i], ordem);
                printf("\n");
            }
        }
    }
}

void exibirPalavrasComNroOcorPorLetra(Avl *nodo, int ocor)
{
    if (nodo != NULL)
    {
        exibirPalavrasComNroOcorPorLetra(nodo->esq, ocor);
        if (nodo->ocor == ocor)
            printf("%s - %d\n", nodo->value, nodo->ocor);
        exibirPalavrasComNroOcorPorLetra(nodo->dir, ocor);
    }
}

void exibirPalavrasComNroOcor(Avl *h[], int ocor)
{
    int i;
    for (i = 0; i < 26; i++)
    {
        if (h[i] != NULL)
        {
            exibirPalavrasComNroOcorPorLetra(h[i], ocor);
        }
    }
}

// Reduz a ocorrência de uma palavra em uma unidade deletando caso chege a 0
void reduzirOcorrencia(char *val, Avl **root, Avl *h[])
{
    if (*root == NULL)
    {
        printf("A palavra %s não tem ocorrencias.\n", val);
        return;
    }

    if (strcmp(val, (*root)->value) == 0)
    {
        if ((*root)->ocor > 1)
        {
            (*root)->ocor -= 1;
        }
        else
        {
            removerValor(val, h);
        }
    }
    else if (strcmp(val, (*root)->value) > 0)
    {
        reduzirOcorrencia(val, &(*root)->dir, h);
    }
    else
    {
        reduzirOcorrencia(val, &(*root)->esq, h);
    }
}
int getMaiorNumOcorLetra(Avl *nodo)
{
    int esq, dir, max;
    if (nodo != NULL)
    {
        esq = getMaiorNumOcorLetra(nodo->esq);
        dir = getMaiorNumOcorLetra(nodo->dir);
        max = dir > esq ? dir : esq;
        return nodo->ocor > max ? nodo->ocor : max;
    }
    else
    {
        return 0;
    }
}

int getMaiorNumOcor(Avl *h[])
{
    int i, maior = 0, aux;
    for (i = 0; i < 26; i++)
    {
        if (h[i] != NULL)
        {
            aux = getMaiorNumOcorLetra(h[i]);
            if (aux > maior)
            {
                maior = aux;
            }
        }
    }

    return maior;
}

void showMenu()
{
    printf("Menu:\n");
    printf("0. Sair\n");
    printf("1. Inserir mais texto\n");
    printf("2. Limpar textos\n");
    printf("3. Buscar palavra\n");
    printf("4. Remover palavra\n");
    printf("5. Mostrar total de palavras\n");
    printf("6. Mostrar total de palavras diferentes\n");
    printf("7. Exibir lista de palavras em ordem alfabetica\n");
    printf("8. Exibir palavras por letra\n");
    printf("9. Exibir palavras com maior numero de ocorrencias\n");
    printf("10. Mostrar palavra de ocorrencia unica\n");
    printf("11. Decrementar ocorrencias de uma palavra\n");
    printf("\n");
}

void main()
{
    // Inicia a tabela hash
    Avl *letterHashTable[26];
    init(letterHashTable);

    // Obtém a frase
    char text[5000];
    printf("Informe o texto:\n");
    gets(text);

    // Declara variáveis de uso geral
    int opt, subopt;
    Avl *aux;

    // Processa frase
    getJustWords(text, letterHashTable);

    do
    {
        showMenu();

        scanf("%d", &opt);
        switch (opt)
        {
        case 1:
            printf("\nInforme o texto:\n");
            // Limpa o buffer
            gets(text);

            // Le  o texto
            gets(text);

            getJustWords(text, letterHashTable);
            break;
        case 2:
            init(letterHashTable);
            printf("palavras removidas!\n\n");
            break;
        case 3:
            printf("\nInforme a palavra a ser buscada:\n");
            scanf("%s", text);
            consultaOcorrencias(text, letterHashTable[text[0] - 'a']);
            printf("\n");
            break;
        case 4:
            printf("\nInforme a palavra a ser removida:\n");
            scanf("%s", &text);
            removerValor(text, letterHashTable);
            break;
        case 5:
            printf("\nTotal de palavras: %d\n\n", getOcorrencias(letterHashTable));
            break;
        case 6:
            printf("\nTotal de palavras diferentes: %d\n\n", getTamanho(letterHashTable));
            break;
        case 7:

            // Le a ordem de ordenacao
            do
            {
                printf("\nSelecione a ordem:\n");
                printf("1. A-Z\n");
                printf("2. Z-A\n");
                scanf("%d", &subopt);
            } while (subopt != 1 && subopt != 2);
            printf("\n");
            // Escreve
            if (subopt == 1)
            {
                // A-Z
                exibirPalavras(letterHashTable, 1);
            }
            else
            {
                // Z-A
                exibirPalavras(letterHashTable, 0);
            }
            printf("\n");
            break;
        case 8:
            printf("Informe a letra a ser buscada:\n");
            scanf("%s", text);

            exibirPalavrasPorLetra(&(letterHashTable[text[0] - 'a']), 1);
            printf("\n");
            break;
        case 9:
            exibirPalavrasComNroOcor(letterHashTable, getMaiorNumOcor(letterHashTable));
            printf("\n");
            break;
        case 10:
            exibirPalavrasComNroOcor(letterHashTable, 1);
            printf("\n");
            break;
        case 11:
            printf("Informe a palavra a ter seu número de ocorrencias decrementado:\n");
            scanf("%s", text);
            reduzirOcorrencia(text, &letterHashTable[text[0] - 'a'], letterHashTable);
            printf("\n");
            break;
        }
    } while (opt != 0);
}
