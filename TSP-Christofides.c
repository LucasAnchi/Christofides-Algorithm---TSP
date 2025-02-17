#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <assert.h>

typedef struct {
    double x, y;
} Cidade;

typedef struct {
    int* vertices;
    int tamanho, capacidade;
} ListaAdjacencia;

typedef struct {
    ListaAdjacencia* lista;
} Multigrafo;

typedef struct {
    int* dados;
    int topo, capacidade;
} Pilha;

Cidade* lerArquivo(char* nomeArquivo, int numCidades);

int pesoMin(double peso[], bool mstSet[], int numVerts);
int* MST(double** matriz, int numCidades);

int* IsolarVerticesImpares(int parent[], int* count, int numCidades);

int** EmpMin(double** dist, int* vertsImp, int count, int* qntPares, int numCidades);

void initMultigrafo(Multigrafo* mg, int numCidades);
void addAresta(Multigrafo* mg, int u, int v);
int removerAresta(Multigrafo* mg, int u);
void Multi(Multigrafo* mg, int* mst, int** Emp, int pares, int numCidades);

void initPilha(Pilha* p);
void empilhar(Pilha* p, int valor);
int desempilhar(Pilha* p);
int topoPilha(Pilha* p);
int pilhaVazia(Pilha* p);
void hierholzer(int inicio, Multigrafo* mg, Pilha* circuito, int numCidades);

void shortcutCircuit(int* circuito, int tamanhoCircuito, int* tour, int* tamanhoTour, int numCidades);
double calcularDistancia(int* tour, int tamanhoTour, double** distancias, int numCidades);

void ChristofidesAlgorithm(double** distancias, int numCidades);

// Função para calcular a distância euclidiana entre duas cidades
double distEuclidiana(Cidade cidade1, Cidade cidade2) {
    return sqrt(pow((cidade1.x - cidade2.x), 2) + pow((cidade1.y - cidade2.y), 2));
}

// Função para criar a matriz de distâncias
void matrizDistancias(Cidade* cidades, double** matriz, int numCidades) {
    for (int i = 0; i < numCidades; i++)
        for (int j = 0; j < numCidades; j++)
            matriz[i][j] = distEuclidiana(cidades[i], cidades[j]);
}

Cidade* lerArquivo(char* nomeArquivo, int numCidades) {
    FILE* cenario;
    int cidadeslidas = 0;
    char buffer[100];
    Cidade* coordenadascidades = malloc(numCidades * sizeof(Cidade));

    cenario = fopen(nomeArquivo, "r");
    if (cenario == NULL) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    while (fgets(buffer, sizeof(buffer), cenario)) {
        int id;
        double x, y;
        if (sscanf(buffer, "%d %lf %lf", &id, &x, &y) == 3) {
            coordenadascidades[cidadeslidas].x = x;
            coordenadascidades[cidadeslidas].y = y;
            cidadeslidas++;
        }
    }

    fclose(cenario);
    return coordenadascidades;
}

int pesoMin(double peso[], bool verts[], int numVerts) {
    double min = DBL_MAX;
    int min_index = -1;

    for (int v = 0; v < numVerts; v++) {
        if (!verts[v] && peso[v] < min) {
            min = peso[v];
            min_index = v;
        }
    }
    return min_index;
}

int* MST(double** matriz, int numCidades) {
    int* caminho = malloc(numCidades * sizeof(int));
    assert(caminho);

    double peso[numCidades];
    bool visitado[numCidades];
    for(int i = 0; i < numCidades;i++){
        visitado[i] = false;
    }

    for (int i = 0; i < numCidades; i++) {
        peso[i] = DBL_MAX;
        caminho[i] = -1;
    }

    peso[0] = 0.0;
    for (int count = 0; count < numCidades - 1; count++) {
        int u = pesoMin(peso, visitado, numCidades);
        if (u == -1) { break; }

        visitado[u] = true;

        for (int v = 0; v < numCidades; v++) {
            if (matriz[u][v] > 0 && !visitado[v] && matriz[u][v] < peso[v]) {
                caminho[v] = u;
                peso[v] = matriz[u][v];
            }
        }
    }

    return caminho;
}

int* IsolarVerticesImpares(int caminhoMST[], int* count, int numCidades) {
    int grau[numCidades];
    for(int i = 0; i < numCidades;i++){
        grau[i] = 0;
    }
    *count = 0;

    for (int i = 1; i < numCidades; i++) {
        int u = caminhoMST[i];
        int v = i;
        grau[u]++;
        grau[v]++;
    }

    for (int i = 0; i < numCidades; i++) {
        if (grau[i] % 2 != 0) {
            (*count)++;
        }
    }

    int* VetImpares = malloc((*count) * sizeof(int));
    int aux = 0;
    for (int i = 0; i < numCidades; i++) {
        if (grau[i] % 2 != 0) {
            VetImpares[aux++] = i;
        }
    }

    return VetImpares;
}

int** EmpMin(double** dist, int* vertsImp, int count, int* qntPares, int numCidades) {
    int** pares = (int**)malloc((count / 2) * sizeof(int*));
    if (pares == NULL) {
        free(pares);
        return NULL;
    }
    *qntPares = 0;
    bool* visitado = (bool*)calloc(count, sizeof(bool));

    for (int i = 0; i < count; i++) {
        if (!visitado[i]) {
            double min_dist = DBL_MAX;
            int melhorPar = -1;
            for (int j = i + 1; j < count; j++) {
                if (!visitado[j]) {
                    double current_dist = dist[vertsImp[i]][vertsImp[j]];
                    if (current_dist < min_dist) {
                        min_dist = current_dist;
                        melhorPar = j;
                    }
                }
            }
            if (melhorPar != -1) {
                pares[*qntPares] = (int*)malloc(2 * sizeof(int));
                pares[*qntPares][0] = vertsImp[i];
                pares[*qntPares][1] = vertsImp[melhorPar];
                visitado[i] = true;
                visitado[melhorPar] = true;
                (*qntPares)++;
            }
        }
    }
    free(visitado);
    return pares;
}

void initMultigrafo(Multigrafo* mg, int numCidades) {
    mg->lista = (ListaAdjacencia*)malloc(numCidades * sizeof(ListaAdjacencia));
    for (int i = 0; i < numCidades; i++) {
        mg->lista[i].vertices = NULL;
        mg->lista[i].tamanho = 0;
        mg->lista[i].capacidade = 0;
    }
}

void addAresta(Multigrafo* mg, int u, int v) {
    if (mg->lista[u].tamanho >= mg->lista[u].capacidade) {
        int nvCap = (mg->lista[u].capacidade == 0) ? 4 : mg->lista[u].capacidade * 2;
        mg->lista[u].vertices = realloc(mg->lista[u].vertices, nvCap * sizeof(int));
        mg->lista[u].capacidade = nvCap;
    }

    mg->lista[u].vertices[mg->lista[u].tamanho++] = v;

    if (mg->lista[v].tamanho >= mg->lista[v].capacidade) {
        int nvCap = (mg->lista[v].capacidade == 0) ? 4 : mg->lista[v].capacidade * 2;
        mg->lista[v].vertices = realloc(mg->lista[v].vertices, nvCap * sizeof(int));
        mg->lista[v].capacidade = nvCap;
    }
    mg->lista[v].vertices[mg->lista[v].tamanho++] = u;
}

void Multi(Multigrafo* mg, int* mst, int** emp, int pares, int numCidades) {
    for (int i = 1; i < numCidades; i++) {
        int caminho = mst[i];
        addAresta(mg, caminho, i);
    }

    for (int i = 0; i < pares; i++) {
        int u = emp[i][0];
        int v = emp[i][1];
        addAresta(mg, u, v);
    }
}

void initPilha(Pilha* p) {
    p->capacidade = 16;
    p->dados = malloc(p->capacidade * sizeof(int));
    p->topo = -1;
}

void empilhar(Pilha* p, int valor) {
    if (p->topo == p->capacidade - 1) {
        p->capacidade *= 2;
        p->dados = realloc(p->dados, p->capacidade * sizeof(int));
    }
    p->dados[++p->topo] = valor;
}

int desempilhar(Pilha* p) {
    if (p->topo == -1) return -1;
    return p->dados[p->topo--];
}

int topoPilha(Pilha* p) {
    if (p->topo == -1) return -1;
    return p->dados[p->topo];
}

int pilhaVazia(Pilha* p) {
    return p->topo == -1;
}

int removerAresta(Multigrafo* mg, int u) {
    if (mg->lista[u].tamanho == 0) return -1;
    int v = mg->lista[u].vertices[mg->lista[u].tamanho - 1];
    mg->lista[u].tamanho--;

    for (int i = 0; i < mg->lista[v].tamanho; i++) {
        if (mg->lista[v].vertices[i] == u) {
            mg->lista[v].vertices[i] = mg->lista[v].vertices[mg->lista[v].tamanho - 1];
            mg->lista[v].tamanho--;
            break;
        }
    }
    return v;
}

void hierholzer(int inicio, Multigrafo* mg, Pilha* circuito, int numCidades) {
    Pilha pilha;
    initPilha(&pilha);
    empilhar(&pilha, inicio);

    int max_iterations = 1000;
    int iterations = 0;

    while (!pilhaVazia(&pilha)) {
        if (iterations++ > max_iterations) {
            printf("Loop infinito detectado!\n");
            break;
        }

        int u = topoPilha(&pilha);
        if (mg->lista[u].tamanho > 0) {
            int v = removerAresta(mg, u);
            empilhar(&pilha, v);
        } else {
            empilhar(circuito, desempilhar(&pilha));
        }
    }
}

void shortcutCircuit(int* circuito, int tamanhoCircuito, int* tour, int* tamanhoTour, int numCidades) {
    bool visitado[numCidades];
    for(int i = 0; i < numCidades;i++){
        visitado[i] = false;
    }
    *tamanhoTour = 0;
    for (int i = 0; i < tamanhoCircuito; i++) {
        int cidade = circuito[i];
        if (!visitado[cidade]) {
            tour[*tamanhoTour] = cidade;
            (*tamanhoTour)++;
            visitado[cidade] = true;
        }
    }
    tour[*tamanhoTour] = tour[0];
    (*tamanhoTour)++;
}

double calcularDistancia(int* tour, int tamanhoTour, double** distancias, int numCidades) {
    double distanciaTotal = 0.0;
    for (int i = 0; i < tamanhoTour - 1; i++) {
        distanciaTotal += distancias[tour[i]][tour[i + 1]];
    }
    return distanciaTotal;
}

void ChristofidesAlgorithm(double** distancias, int numCidades) {
    int* caminhoMST = MST(distancias, numCidades);
    int count;
    int* VertImpar = IsolarVerticesImpares(caminhoMST, &count, numCidades);
    int numPares;
    int** paresEmparelhados = EmpMin(distancias, VertImpar, count, &numPares, numCidades);
    Multigrafo multigrafo;
    initMultigrafo(&multigrafo, numCidades);
    Multi(&multigrafo, caminhoMST, paresEmparelhados, numPares, numCidades);
    Pilha camEuler;
    initPilha(&camEuler);
    int inicio = (count > 0) ? VertImpar[0] : 0;
    hierholzer(inicio, &multigrafo, &camEuler, numCidades);
    int* camEulerArray = malloc((camEuler.topo + 1) * sizeof(int));
    for (int i = camEuler.topo; i >= 0; i--) {
        camEulerArray[camEuler.topo - i] = camEuler.dados[i];
    }
    int tamanhoCircuito = camEuler.topo + 1;
    int tourTSP[numCidades + 1];
    int tamanhoTour;
    shortcutCircuit(camEulerArray, tamanhoCircuito, tourTSP, &tamanhoTour, numCidades);
    double distanciaTotal = calcularDistancia(tourTSP, tamanhoTour, distancias, numCidades);
    printf("\nTour TSP:\n");
    for (int i = 0; i < tamanhoTour; i++) {
        printf("%d", tourTSP[i]);
        if (i < tamanhoTour - 1) printf(" -> ");
    }
    printf("\nDistância Total: %.2f\n", distanciaTotal);
    free(camEulerArray);
    free(VertImpar);
    free(caminhoMST);
    for (int i = 0; i < numPares; i++) {
        free(paresEmparelhados[i]);
    }
    free(paresEmparelhados);
    for (int i = 0; i < numCidades; i++) {
        free(multigrafo.lista[i].vertices);
    }
    free(multigrafo.lista);
}

int main() {
    char arquivo[50];
    int numCidades;

    printf("Digite o nome do arquivo: ");
    scanf("%s", arquivo);
    printf("Digite o numero de cidades: ");
    scanf("%d", &numCidades);
    Cidade* cidades = lerArquivo(arquivo, numCidades);

    double** distancias = malloc(numCidades * sizeof(double*));
    for (int i = 0; i < numCidades; i++) {
        distancias[i] = malloc(numCidades * sizeof(double));
    }

    matrizDistancias(cidades, distancias, numCidades);
    ChristofidesAlgorithm(distancias, numCidades);
    free(cidades);
    for (int i = 0; i < numCidades; i++) {
        free(distancias[i]);
    }
    free(distancias);
    return 0;
}