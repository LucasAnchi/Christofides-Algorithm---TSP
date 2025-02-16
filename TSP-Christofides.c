#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <assert.h>

#define NUM_CIDADES 318

typedef struct {
    double x, y;
}Cidade;

typedef struct 
{
    int* vertices;
    int tamanho, capacidade;
} ListaAdjacencia;

typedef struct{
    ListaAdjacencia* lista;
}Multigrafo;

typedef struct 
{
    int* dados;
    int topo, capacidade;
}Pilha;

int pesoMin(double peso[], bool mstSet[],int numVerts);
int* MST(double matriz[NUM_CIDADES][NUM_CIDADES]);

int* IsolarVerticesImpares(int parent[], int* count);

int** EmpMin(double dist[NUM_CIDADES][NUM_CIDADES],int* vertsImp,int count,int* qntPares);

void initMultigrafo(Multigrafo* mg);
void addAresta(Multigrafo* mg,int u,int v);
int removerAresta(Multigrafo* mg,int u);
void Multi(Multigrafo* mg, int* mst, int** Emp, int pares);

void initPilha(Pilha* p);
void empilhar(Pilha* p, int valor);
int desempilhar(Pilha* p);
int topoPilha(Pilha* p);
int pilhaVazia(Pilha* p);
void hierholzer(int inicio, Multigrafo* mg, Pilha* circuito);

void shortcutCircuit(int* circuito, int tamanhoCircuito, int* tour, int* tamanhoTour);
double calcularDistancia(int* tour, int tamanhoTour, double distancias[NUM_CIDADES][NUM_CIDADES]);

void ChristofidesAlgorithm(double distancia[NUM_CIDADES][NUM_CIDADES]);

// Função para calcular a distância euclidiana entre duas cidades
double distEuclidiana(Cidade cidade1, Cidade cidade2) {
    return sqrt(pow((cidade1.x - cidade2.x),2) + pow((cidade1.y - cidade2.y),2));
}

// Função para criar a matriz de distâncias
void matrizDistancias(Cidade cidades[], double matriz[NUM_CIDADES][NUM_CIDADES]) {
    for (int i = 0; i < NUM_CIDADES; i++)
        for (int j = 0; j < NUM_CIDADES; j++) 
            matriz[i][j] = distEuclidiana(cidades[i], cidades[j]);
}

//Valor Chave minimo do conjunto de vertices
int pesoMin(double peso[], bool verts[],int numVerts){

    double min = DBL_MAX; 
    int min_index = -1;

    for (int v = 0; v < numVerts;v++){
        if (!verts[v] && peso[v] < min) {
            min = peso[v];
            min_index = v;
        }    
    }
    return min_index;
}

int* MST(double matriz[NUM_CIDADES][NUM_CIDADES]){
    int* caminho = malloc(NUM_CIDADES * sizeof(int));
    assert(caminho);

    double peso[NUM_CIDADES];
    bool visitado[NUM_CIDADES] = {false};

    for(int i = 0; i < NUM_CIDADES;i++) {
        peso[i] = DBL_MAX;
        caminho[i] = -1;
    }

    peso[0] = 0.0;
    for(int count = 0; count < NUM_CIDADES-1;count++){
        int u = pesoMin(peso,visitado,NUM_CIDADES);
        if(u == -1) {break;}

        visitado[u] = true;

        for(int v = 0;v < NUM_CIDADES;v++){
            if(matriz[u][v] > 0 && !visitado[v] && matriz[u][v] < peso[v]){
                caminho[v] = u;
                peso[v] = matriz[u][v];
            }
        }
    }

    return caminho;
}

int* IsolarVerticesImpares(int caminhoMST[], int* count){
    int grau[NUM_CIDADES] = {0};
    *count = 0;

    //calcular o grau de cada vertice
    for (int i = 1; i < NUM_CIDADES; i++) {
        int u = caminhoMST[i]; // Pai do vértice i na MST
        int v = i;         // Vértice i
        grau[u]++;         // Incrementa o grau do pai
        grau[v]++;         // Incrementa o grau do vértice
    }

    for (int i = 0; i < NUM_CIDADES; i++) {
        if (grau[i] % 2 != 0) {
            (*count)++;
        }
    }

    int* VetImpares = malloc((*count)*sizeof(int));
    int aux = 0;
    for(int i = 0; i < NUM_CIDADES;i++){
        if (grau[i] % 2 != 0) {
            VetImpares[aux++] = i;
        }
    }

    return VetImpares;
}

int** EmpMin(double dist[NUM_CIDADES][NUM_CIDADES], int* vertsImp, int count, int* qntPares) {
    int** pares = (int**)malloc((count/2) * sizeof(int*));
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

void initMultigrafo(Multigrafo* mg){
    mg->lista = (ListaAdjacencia*)malloc(NUM_CIDADES * sizeof(ListaAdjacencia));
    for(int i = 0;i < NUM_CIDADES;i++){
        mg->lista[i].vertices = NULL;
        mg->lista[i].tamanho = 0;
        mg->lista[i].capacidade = 0;
    }
}

void addAresta(Multigrafo* mg, int u, int v){
    if(mg->lista[u].tamanho >= mg->lista[u].capacidade){
        int nvCap = (mg->lista[u].capacidade == 0) ? 4: mg->lista[u].capacidade * 2;
        mg->lista[u].vertices = realloc(mg->lista[u].vertices, nvCap * sizeof(int));
        mg->lista[u].capacidade = nvCap;
    }

    mg->lista[u].vertices[mg->lista[u].tamanho++] = v;

    if(mg->lista[v].tamanho >= mg->lista[v].capacidade){
        int nvCap = (mg->lista[v].capacidade == 0) ? 4: mg->lista[v].capacidade*2;
        mg->lista[v].vertices = realloc(mg->lista[v].vertices,nvCap * sizeof(int));
        mg->lista[v].capacidade = nvCap;
    }
    mg->lista[v].vertices[mg->lista[v].tamanho++] = u;
}

void Multi(Multigrafo* mg, int* mst, int** emp, int pares){
    for(int i = 1;i < NUM_CIDADES;i++){
        int caminho = mst[i];
        addAresta(mg,caminho,i);
    }

    for(int i = 0;i < pares;i++){
        int u = emp[i][0];
        int v = emp[i][1];
        addAresta(mg,u,v);
    }
}

void initPilha(Pilha* p){
    p->capacidade = 16;
    p->dados = malloc(p->capacidade*sizeof(int));
    p->topo = -1;
}

void empilhar(Pilha* p, int valor){
    if(p->topo == p->capacidade -1){
        p->capacidade *=2;
        p->dados = realloc(p->dados, p->capacidade*sizeof(int));
    }
    p->dados[++p->topo] = valor;
}

int desempilhar(Pilha* p){
    if(p->topo == -1) return -1;
    return p->dados[p->topo--];
}

int topoPilha(Pilha* p){
    if(p->topo == -1) return -1;
    return p->dados[p->topo];
}

int pilhaVazia(Pilha* p){
    return p->topo == -1;
}

int removerAresta(Multigrafo* mg, int u){
    if(mg->lista[u].tamanho == 0) return -1;
    int v = mg->lista[u].vertices[mg->lista[u].tamanho -1];
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

void hierholzer(int inicio, Multigrafo* mg, Pilha* circuito) {
    Pilha pilha;
    initPilha(&pilha);
    empilhar(&pilha, inicio);

    int max_iterations = 1000;
    int iterations = 0;

    while (!pilhaVazia(&pilha)) {
        if(iterations++ > max_iterations){
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

void shortcutCircuit(int* circuito, int tamanhoCircuito, int* tour, int* tamanhoTour) {
    bool visitado[NUM_CIDADES] = {false};
    *tamanhoTour = 0;
    for (int i = 0; i < tamanhoCircuito; i++) {
        int cidade = circuito[i];
        if (!visitado[cidade]) {
            tour[*tamanhoTour] = cidade;
            (*tamanhoTour)++;
            visitado[cidade] = true;
        }
    }
    tour[*tamanhoTour] = tour[0]; // Completa o ciclo
    (*tamanhoTour)++;
}

double calcularDistancia(int* tour, int tamanhoTour, double distancias[NUM_CIDADES][NUM_CIDADES]) {
    double distanciaTotal = 0.0;
    for (int i = 0; i < tamanhoTour - 1; i++) {
        distanciaTotal += distancias[tour[i]][tour[i+1]];
    }
    return distanciaTotal;
}

void ChristofidesAlgorithm(double distancias[NUM_CIDADES][NUM_CIDADES]){
  
    //passo 2.1: Gerar a MST
    int *caminhoMST = MST(distancias);

    //passo 2.2
    int count;
    int *VertImpar = IsolarVerticesImpares(caminhoMST, &count); 

    // Passo 2.3: Emparelhamento mínimo (heurística gulosa)
    int numPares;
    int** paresEmparelhados = EmpMin(distancias, VertImpar, count, &numPares);

    //Passo 2.4: construir Multigrafo
    Multigrafo multigrafo;
    initMultigrafo(&multigrafo);
    Multi(&multigrafo, caminhoMST, paresEmparelhados,  numPares);

    //Passo 2.5: Encontrar o caminho Euleriano
    Pilha camEuler;
    initPilha(&camEuler);
    int inicio = (count > 0) ? VertImpar[0] : 0;
    hierholzer(inicio, &multigrafo, &camEuler);

    //pilha -> array
    int* camEulerArray = malloc((camEuler.topo +1) * sizeof(int));
    for (int i = camEuler.topo; i >= 0; i--) {
        camEulerArray[camEuler.topo - i] = camEuler.dados[i];
    }
    int tamanhoCircuito = camEuler.topo + 1;

    // Passo 2.6: Atalhar para TSP
    int tourTSP[NUM_CIDADES + 1];
    int tamanhoTour;
    shortcutCircuit(camEulerArray, tamanhoCircuito, tourTSP, &tamanhoTour);

    // Calcular distância
    double distanciaTotal = calcularDistancia(tourTSP, tamanhoTour, distancias);

    // Imprimir resultados
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
    for (int i = 0; i < NUM_CIDADES; i++) {
        free(multigrafo.lista[i].vertices);
    }
    free(multigrafo.lista);
}

int main() {
    // Coordenadas das cidades (extraídas do arquivo)
    Cidade cidades[NUM_CIDADES] = {
        {63.0, 71.0},
        {94.0, 71.0},
        {142.0, 370.0},
        {173.0, 1276.0},
        {205.0, 1213.0},
        {213.0, 69.0},
        {244.0, 69.0},
        {276.0, 630.0},
        {283.0, 732.0},
        {362.0, 69.0},
        {394.0, 69.0},
        {449.0, 370.0},
        {480.0, 1276.0},
        {512.0, 1213.0},
        {528.0, 157.0},
        {583.0, 630.0},
        {591.0, 732.0},
        {638.0, 654.0},
        {638.0, 496.0},
        {638.0, 314.0},
        {638.0, 142.0},
        {669.0, 142.0},
        {677.0, 315.0},
        {677.0, 496.0},
        {677.0, 654.0},
        {709.0, 654.0},
        {709.0, 496.0},
        {709.0, 315.0},
        {701.0, 142.0},
        {764.0, 220.0},
        {811.0, 189.0},
        {843.0, 173.0},
        {858.0, 370.0},
        {890.0, 1276.0},
        {921.0, 1213.0},
        {992.0, 630.0},
        {1000.0, 732.0},
        {1197.0, 1276.0},
        {1228.0, 1213.0},
        {1276.0, 205.0},
        {1299.0, 630.0},
        {1307.0, 732.0},
        {1362.0, 654.0},
        {1362.0, 496.0},
        {1362.0, 291.0},
        {1425.0, 654.0},
        {1425.0, 496.0},
        {1425.0, 291.0},
        {1417.0, 173.0},
        {1488.0, 291.0},
        {1488.0, 496.0},
        {1488.0, 654.0},
        {1551.0, 654.0},
        {1551.0, 496.0},
        {1551.0, 291.0},
        {1614.0, 291.0},
        {1614.0, 496.0},
        {1614.0, 654.0},
        {1732.0, 189.0},
        {1811.0, 1276.0},
        {1843.0, 1213.0},
        {1913.0, 630.0},
        {1921.0, 732.0},
        {2087.0, 370.0},
        {2118.0, 1276.0},
        {2150.0, 1213.0},
        {2189.0, 205.0},
        {2220.0, 189.0},
        {2220.0, 630.0},
        {2228.0, 732.0},
        {2244.0, 142.0},
        {2276.0, 315.0},
        {2276.0, 496.0},
        {2276.0, 654.0},
        {2315.0, 654.0},
        {2315.0, 496.0},
        {2315.0, 315.0},
        {2331.0, 142.0},
        {2346.0, 315.0},
        {2346.0, 496.0},
        {2346.0, 654.0},
        {2362.0, 142.0},
        {2402.0, 157.0},
        {2402.0, 220.0},
        {2480.0, 142.0},
        {2496.0, 370.0},
        {2528.0, 1276.0},
        {2559.0, 1213.0},
        {2630.0, 630.0},
        {2638.0, 732.0},
        {2756.0, 69.0},
        {2787.0, 69.0},
        {2803.0, 370.0},
        {2835.0, 1276.0},
        {2866.0, 1213.0},
        {2906.0, 69.0},
        {2937.0, 69.0},
        {2937.0, 630.0},
        {2945.0, 732.0},
        {3016.0, 1276.0},
        {3055.0, 69.0},
        {3087.0, 69.0},
        {606.0, 220.0},
        {1165.0, 370.0},
        {1780.0, 370.0},
        {63.0, 1402.0},
        {94.0, 1402.0},
        {142.0, 1701.0},
        {173.0, 2607.0},
        {205.0, 2544.0},
        {213.0, 1400.0},
        {244.0, 1400.0},
        {276.0, 1961.0},
        {283.0, 2063.0},
        {362.0, 1400.0},
        {394.0, 1400.0},
        {449.0, 1701.0},
        {480.0, 2607.0},
        {512.0, 2544.0},
        {528.0, 1488.0},
        {583.0, 1961.0},
        {591.0, 2063.0},
        {638.0, 1985.0},
        {638.0, 1827.0},
        {638.0, 1645.0},
        {638.0, 1473.0},
        {669.0, 1473.0},
        {677.0, 1646.0},
        {677.0, 1827.0},
        {677.0, 1985.0},
        {709.0, 1985.0},
        {709.0, 1827.0},
        {709.0, 1646.0},
        {701.0, 1473.0},
        {764.0, 1551.0},
        {811.0, 1520.0},
        {843.0, 1504.0},
        {858.0, 1701.0},
        {890.0, 2607.0},
        {921.0, 2544.0},
        {992.0, 1961.0},
        {1000.0, 2063.0},
        {1197.0, 2607.0},
        {1228.0, 2544.0},
        {1276.0, 1536.0},
        {1299.0, 1961.0},
        {1307.0, 2063.0},
        {1362.0, 1985.0},
        {1362.0, 1827.0},
        {1362.0, 1622.0},
        {1425.0, 1985.0},
        {1425.0, 1827.0},
        {1425.0, 1622.0},
        {1417.0, 1504.0},
        {1488.0, 1622.0},
        {1488.0, 1827.0},
        {1488.0, 1985.0},
        {1551.0, 1985.0},
        {1551.0, 1827.0},
        {1551.0, 1622.0},
        {1614.0, 1622.0},
        {1614.0, 1827.0},
        {1614.0, 1985.0},
        {1732.0, 1520.0},
        {1811.0, 2607.0},
        {1843.0, 2544.0},
        {1913.0, 1961.0},
        {1921.0, 2063.0},
        {2087.0, 1701.0},
        {2118.0, 2607.0},
        {2150.0, 2544.0},
        {2189.0, 1536.0},
        {2220.0, 1520.0},
        {2220.0, 1961.0},
        {2228.0, 2063.0},
        {2244.0, 1473.0},
        {2276.0, 1646.0},
        {2276.0, 1827.0},
        {2276.0, 1985.0},
        {2315.0, 1985.0},
        {2315.0, 1827.0},
        {2315.0, 1646.0},
        {2331.0, 1473.0},
        {2346.0, 1646.0},
        {2346.0, 1827.0},
        {2346.0, 1985.0},
        {2362.0, 1473.0},
        {2402.0, 1488.0},
        {2402.0, 1551.0},
        {2480.0, 1473.0},
        {2496.0, 1701.0},
        {2528.0, 2607.0},
        {2559.0, 2544.0},
        {2630.0, 1961.0},
        {2638.0, 2063.0},
        {2756.0, 1400.0},
        {2787.0, 1400.0},
        {2803.0, 1701.0},
        {2835.0, 2607.0},
        {2866.0, 2544.0},
        {2906.0, 1400.0},
        {2937.0, 1400.0},
        {2937.0, 1961.0},
        {2945.0, 2063.0},
        {3016.0, 2607.0},
        {3055.0, 1400.0},
        {3087.0, 1400.0},
        {606.0, 1551.0},
        {1165.0, 1701.0},
        {1780.0, 1701.0},
        {63.0, 2733.0},
        {94.0, 2733.0},
        {142.0, 3032.0},
        {173.0, 3938.0},
        {205.0, 3875.0},
        {213.0, 2731.0},
        {244.0, 2731.0},
        {276.0, 3292.0},
        {283.0, 3394.0},
        {362.0, 2731.0},
        {394.0, 2731.0},
        {449.0, 3032.0},
        {480.0, 3938.0},
        {512.0, 3875.0},
        {528.0, 2819.0},
        {583.0, 3292.0},
        {591.0, 3394.0},
        {638.0, 3316.0},
        {638.0, 3158.0},
        {638.0, 2976.0},
        {638.0, 2804.0},
        {669.0, 2804.0},
        {677.0, 2977.0},
        {677.0, 3158.0},
        {677.0, 3316.0},
        {709.0, 3316.0},
        {709.0, 3158.0},
        {709.0, 2977.0},
        {701.0, 2804.0},
        {764.0, 2882.0},
        {811.0, 2851.0},
        {843.0, 2835.0},
        {858.0, 3032.0},
        {890.0, 3938.0},
        {921.0, 3875.0},
        {992.0, 3292.0},
        {1000.0, 3394.0},
        {1197.0, 3938.0},
        {1228.0, 3875.0},
        {1276.0, 2867.0},
        {1299.0, 3292.0},
        {1307.0, 3394.0},
        {1362.0, 3316.0},
        {1362.0, 3158.0},
        {1362.0, 2953.0},
        {1425.0, 3316.0},
        {1425.0, 3158.0},
        {1425.0, 2953.0},
        {1417.0, 2835.0},
        {1488.0, 2953.0},
        {1488.0, 3158.0},
        {1488.0, 3316.0},
        {1551.0, 3316.0},
        {1551.0, 3158.0},
        {1551.0, 2953.0},
        {1614.0, 2953.0},
        {1614.0, 3158.0},
        {1614.0, 3316.0},
        {1732.0, 2851.0},
        {1811.0, 3938.0},
        {1843.0, 3875.0},
        {1913.0, 3292.0},
        {1921.0, 3394.0},
        {2087.0, 3032.0},
        {2118.0, 3938.0},
        {2150.0, 3875.0},
        {2189.0, 2867.0},
        {2220.0, 2851.0},
        {2220.0, 3292.0},
        {2228.0, 3394.0},
        {2244.0, 2804.0},
        {2276.0, 2977.0},
        {2276.0, 3158.0},
        {2276.0, 3316.0},
        {2315.0, 3316.0},
        {2315.0, 3158.0},
        {2315.0, 2977.0},
        {2331.0, 2804.0},
        {2346.0, 2977.0},
        {2346.0, 3158.0},
        {2346.0, 3316.0},
        {2362.0, 2804.0},
        {2402.0, 2819.0},
        {2402.0, 2882.0},
        {2480.0, 2804.0},
        {2496.0, 3032.0},
        {2528.0, 3938.0},
        {2559.0, 3875.0},
        {2630.0, 3292.0},
        {2638.0, 3394.0},
        {2756.0, 2731.0},
        {2787.0, 2731.0},
        {2803.0, 3032.0},
        {2835.0, 3938.0},
        {2866.0, 3875.0},
        {2906.0, 2731.0},
        {2937.0, 2731.0},
        {2937.0, 3292.0},
        {2945.0, 3394.0},
        {3016.0, 3938.0},
        {3055.0, 2731.0},
        {3087.0, 2731.0},
        {606.0, 2882.0},
        {1165.0, 3032.0},
        {1780.0, 3032.0},
        {1417.0, -79.0},
        {1496.0, -79.0},
        {1693.0, 4055.0}
    };

    // Matriz para armazenar as distâncias
    double distancias[NUM_CIDADES][NUM_CIDADES];

    // Criar a matriz de distâncias
    matrizDistancias(cidades, distancias);

    ChristofidesAlgorithm(distancias);
    
    return 0;
}