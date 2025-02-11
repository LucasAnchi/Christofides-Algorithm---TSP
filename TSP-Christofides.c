#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

#define NUM_CIDADES 10

// Estrutura para armazenar as coordenadas de uma cidade
typedef struct {
    double x; // Longitude
    double y; // Latitude
} Cidade;

//passo 1 criar uma matriz de distancias com as coordenadas dadas nos aquivos docx

//passo 2 utilizar um algoritimo heuristico (nesse caso, algoritimo de Christofides) para descobrir a melhor rota para o TSP
//passo 2.1 gerar uma MST (Minimum Spanning Tree), nesse caso utilizando o Prim’s Algorithm
//passo 2.2 isolar os vertices q possuem grau(qnt de saidas/arestas) impar
//passo 2.3 utilizar outro algoritmo heuristico de baixo processamento para achar a menor distancia entre os vertices isolados(n sei se esse eh o termo pra algoritmo mais eficiente)
//passo 2.4 formar um multigrafo dos passos 2.1 e 2.3 (significa juntar os dois caminhos feitos por eles)
//definicao de multigrafo: é um grafo que pode ter arestas múltiplas, ou seja, arestas que conectam os mesmos vértices
//passo 2.5 achar o caminho Euleriano do grafico/matriz
//definicao de caminho Euleriano: é um caminho em um grafo que visita toda aresta exatamente uma vez.
//ps: da pra fazer isso pq qnd junta os dois graficos no passo 2.4 a gnt conta as arestas que se sobrepoem como duas
//passo 2.6 gerar um caminho TSP (Travelling salesman problem) usando o passo anterior
//se houver alguma cidade (vertice) repetido, pular para o proximo

//passo 3 optimizar esse negocio ai (ou com Ant Colony Optimization ou Simulated Annealing)

int minKey(int key[], bool mstSet[]);
void showMST(int parent[], double matriz[NUM_CIDADES][NUM_CIDADES]);
int* PrimsMST(double matriz[NUM_CIDADES][NUM_CIDADES]);

int* IosolateOdd(int parent[], int* count);

//void VizinhoProximo(int Vert[], double distancia[NUM_CIDADES][NUM_CIDADES]);
void ChristofidesAlgorithm(double distancia[NUM_CIDADES][NUM_CIDADES]);


// Função para calcular a distância euclidiana entre duas cidades
double calcularDistanciaEuclidiana(Cidade cidade1, Cidade cidade2) {
    double dx = cidade2.x - cidade1.x;
    double dy = cidade2.y - cidade1.y;
    return sqrt(dx * dx + dy * dy);
}

// Função para criar a matriz de distâncias
void criarMatrizDistancias(Cidade cidades[], double matriz[NUM_CIDADES][NUM_CIDADES]) {
    for (int i = 0; i < NUM_CIDADES; i++) {
        for (int j = 0; j < NUM_CIDADES; j++) {
            matriz[i][j] = calcularDistanciaEuclidiana(cidades[i], cidades[j]);
        }
    }
}

//Valor Chave minimo do conjunto de vertices
int minKey(int key[], bool mstSet[]){

    int min = INT_MAX, min_index;

    for (int v = 0; v < NUM_CIDADES;v++){
        if (mstSet[v] == false && key[v] < min) {min = key[v],min_index = v;}
    
    }
    return min_index;
}

//Funcao para mostrar a MST
void showMST(int parent[], double matriz[NUM_CIDADES][NUM_CIDADES]){
    
    printf("Caminho \tCusto\n");
    for(int i = 1; i < NUM_CIDADES; i++){
        printf("%d - %d \t\t%.2f \n",parent[i],i,matriz[parent[i]][i]);
    }
    printf("\n");
    for(int i = 1; i < NUM_CIDADES;i++){
        printf("%d -> %d -> ",parent[i],i);
    }
    printf("\n");
}


//Algoritmo para gerar a MST
int* PrimsMST(double matriz[NUM_CIDADES][NUM_CIDADES]){
    //Array para armazenar a construcao do MST
    static int parent[NUM_CIDADES];

    //Valores chaves usados para selecionar o menor caminho
    int key[NUM_CIDADES];

    //Conjunto dos Vertices
    bool mstSet[NUM_CIDADES];

    //Inicializa todas as Keys como infinito
    for(int i = 0; i < NUM_CIDADES;i++) {
        key[i] = INT_MAX;
        mstSet[i] = false;
    }

    key[0] = 0;
    parent[0] = -1;


    for(int count = 0; count < NUM_CIDADES-1;count++){
        //Pegar a menor distancia do vertice atual para outro que n esteja na MST
        int u = minKey(key,mstSet);

        //adiciona o vertice escolhido para o grupo MST
        mstSet[u] = true;

        //este For serve para atualizar tanto o valor 'key' qnt o indice de 'parent' dos vertices adjacentes ao escolhido
        //Considerando apenas os q n estao inclusos no MST
        for(int v = 0;v < NUM_CIDADES;v++){
            //a matriz[u][v] eh diferente de 0 apenas para os adjacentes
            //PS: nesse caso todos os outros vertices sao adjacentes
            //Ver a matriz das distancias na main()

            //mstSet[v] eh falso para os vertices ainda n inclusos no MST

            //O valor key so sera atualizado se matriz[u][v] < key[V]

            if(matriz[u][v] && mstSet[v] == false && matriz[u][v] < key[v]){
                parent[v] = u;
                key[v] = matriz[u][v];
            }
        }
    }

    //Por fim usamos a funcao showMST para printar ela
    //showMST(parent,matriz);
    return parent;
}

//Isola os Vertices de grau impar
int* IosolateOdd(int parent[], int* count){
    static int VetImpares[NUM_CIDADES];
    int aux[NUM_CIDADES] = {0};
    *count = 0;

    //calcular o grau de cada vertice
    for (int i = 1; i < NUM_CIDADES; i++) {
        int u = parent[i]; // Pai do vértice i na MST
        int v = i;         // Vértice i
        aux[u]++;         // Incrementa o grau do pai
        aux[v]++;         // Incrementa o grau do vértice
    }

    //printf("Vértices com grau ímpar na MST:\n");
    for (int i = 0; i < NUM_CIDADES; i++) {
        if (aux[i] % 2 != 0) { // Se o grau é ímpar
            VetImpares[(*count)++] = i;
        }
    }

    return VetImpares;
}



void ChristofidesAlgorithm(double distancias[NUM_CIDADES][NUM_CIDADES]){

    //passo 2.1: Gerar a MST
    int *parent = PrimsMST(distancias);

    //passo 2.2
    int count;
    int *VertImpar = IosolateOdd(parent, &count); 

    printf("Vértices com grau ímpar:\n");
    for (int i = 0; i < count; i++) {
        printf("%d ", VertImpar[i]);
    }
    printf("\n");

}


int main() {


    // Coordenadas das cidades (extraídas do arquivo)
    Cidade cidades[NUM_CIDADES] = {
        {2.549, 44.303}, // São Luís
        {2.447, 43.463}, // Imperatriz
        {3.385, 44.408}, // Caxias
        {3.783, 45.248}, // Timon
        {2.887, 43.356}, // Balsas
        {3.251, 45.903}, // Barreirinhas
        {2.578, 44.568}, // Bacabal
        {3.873, 46.094}, // Chapadinha
        {2.932, 43.812}, // Santa Inês
        {2.466, 43.479}  // Açailândia
    };

    // Matriz para armazenar as distâncias
    double distancias[NUM_CIDADES][NUM_CIDADES];

    // Criar a matriz de distâncias
    criarMatrizDistancias(cidades, distancias);

    ChristofidesAlgorithm(distancias);

    
    /*
    // Exibir a matriz de distâncias
    printf("Matriz de distâncias (distância euclidiana):\n");
    for (int i = 0; i < NUM_CIDADES; i++) {
        for (int j = 0; j < NUM_CIDADES; j++) {
            printf("%.2f\t", distancias[i][j]);
        }
        printf("\n");
    }
*/
    return 0;
}