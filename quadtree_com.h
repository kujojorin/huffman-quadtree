#include<iostream>
#include <vector>
using namespace std;


// a quadtree que estamos fazendo esta lidando somente com imagems cinzas,por isso somente temos um  unsigned char vai de 0 a 255,portanto
// quando eu fazer a descompactação,a imagegem vai ser cinza,com partes pretas,porque a imagem foi expantida,a parte preta vou tentar melhorar depois




// definição:arredonda um numero dado na entrada para retorna um numero 2^n(n sendo qualquer numero >= 0).
// requer: um numero maior que 0.
// devolve: um numero 2^n mais proximo possivel do numero dado na entrada.
int aproximacao(int numero)
{
    int potencia = 1;
    while (potencia < numero)
        potencia = potencia * 2;
    return potencia;
}
// definição:recebendo uma matriz como entrada com linha l e coluna c,cria uma nova matriz com coluna e linha com tamanho maior que c e l, e o numero do tamanho
//  dever ser o numero de base 2 mais proximo que atende essa condição(de ser maior que c e l)//sei que ficou meio prolixo,depois melhoro
// requer: uma matriz criada usando definições do include vectors
// devolve:uma matriz com tamanho 2^n,com os espaços diferentes da matriz original recebendo 0;
vector<vector<unsigned char>> normalizar_matriz(vector<vector<unsigned char>> matriz)
{
    int linhas = matriz.size();
    int colunas = matriz[0].size();

    int maior = linhas >= colunas ? linhas : colunas;
    int tamanho = aproximacao(maior);

    vector<vector<unsigned char>> matriz_normalizada(tamanho, vector<unsigned char>(tamanho, 0));

    for (int i = 0; i < linhas; i++)
        for (int j = 0; j < colunas; j++)
            matriz_normalizada[i][j] = matriz[i][j];

    return matriz_normalizada;
}
//definição: carrega uma imagem,muda ela para cinza,expande ela e retorna como uma matriz
//requer:um nome,e um numero como refencia para retornar o tamanho da matriz criada
//devolve : uma matriz que representa a imagem com cores cinza.
vector<vector<unsigned char>> carregarImagem(const string &file,int &tamanho)
{

    // funcao mat da biblioteca opencv
    cv::Mat imagem = cv::imread(file, cv::IMREAD_COLOR);

   if (imagem.empty())
    {
        cerr << "Erro ao carregar imagem:" << file << endl;
        return vector<vector<unsigned char>>();
    }

    cout << "Imagem carregada com sucesso" << endl;

    // convertendo a imagem carregada para cinza
    cv::Mat gray;
    cv::cvtColor(imagem, gray, cv::COLOR_BGR2GRAY);
    vector<vector<unsigned char>> mat(gray.rows, vector<unsigned char>(gray.cols));
    for (int i = 0; i < gray.rows; i++)
    {
        for (int j = 0; j < gray.cols; j++)
        {
            mat[i][j] = gray.at<unsigned char>(i, j);
        }
    }
    //auto quadrada = normalizar_matriz(mat);
    vector<vector<unsigned char>>quadrada = normalizar_matriz(mat);
 
    tamanho = quadrada.size();
    return quadrada;
    //caso queiram testar a normalização,usem o codigo abaixo,a normalização não estava funcionando porque a imagem teste já tinha tamanho 2^n,então usem 
    //a foto do dragão para testar
/*
    cv::Mat imagemQuadrada(quadrada.size(), quadrada[0].size(), CV_8UC1);

 for (int i = 0; i < quadrada.size(); i++) {
    for (int j = 0; j < quadrada[0].size(); j++) {
        imagemQuadrada.at<uchar>(i, j) = quadrada[i][j];
    }
}

    cout << "Imagem carregada para cinza com sucesso" << endl;
    // exibir a imagem
    cv::namedWindow("Original", cv::WINDOW_NORMAL);
    cv::imshow("Original", imagemQuadrada);

    cv::waitKey(0);

    cv::namedWindow("Grayscale", cv::WINDOW_NORMAL);
    cv::imshow("Grayscale", gray);

    cv::waitKey(0);
    cv::destroyAllWindows();
*/
}
// definação:verifica se uma parte da matriz tem valores proximos,definindo a aceitabililidade de diferença usando a tolerancia
// requer: uma matriz,um ponto,o tamanho do quadrado que se deseja verificar,e o numero que esse bloco pode der de valores diferentes entre si;
// retorna: se a matriz é homegenea ou não segundo a tolerancia;


bool homegenea(const vector<vector<unsigned char>> & matriz, int x, int y, int tamanho, int tolerancia)
{
    unsigned char menor = 225;
    unsigned char maior = 0;
    // for vai de x a x0 e y a y0,ver se a faixa de numeros é menor que a tolerancia;
    for (int i = x; i < x + tamanho; i++)
        for (int j = y; j < y + tamanho; j++)
        {
            unsigned char valor = matriz[i][j];
            menor = min(menor, valor);
            maior = max(maior, valor);

            if (maior - menor > tolerancia)
                return false;
        }
    return true;
}
//definição: cria uma quadtree para uma imagem representada em uma matriz recursivaamente
//requer:uma matriz,uma coordenada(x,y),o tamanho da imagem,o nivel toleravel de diferença nos pixes
//retorna:uma quadtree  
no *criar_quadtree(const vector<vector<unsigned char>> &matriz, int x, int y, int tamanho, int tolerancia)
{
     no *node = new no();
     node->x = x;
     node->y = y;
    node->tamanho = tamanho;
    if (homegenea(matriz, x, y, tamanho, tolerancia))
    {
        node->folha = true;
        node->cor = matriz[x][y];
        node->tamanho = tamanho;
      
        return node;
    }
    if (tamanho == 1) {

        node->folha = true;
        node->cor = matriz[x][y];
        node->tamanho = tamanho;
        return node;
    }
    int meio = tamanho / 2;

    node->filho[0] = criar_quadtree(matriz, x, y, meio, tolerancia);
    node->filho[1] = criar_quadtree(matriz, x, y + meio, meio, tolerancia);
    node->filho[2] = criar_quadtree(matriz, x + meio, y, meio, tolerancia);
    node->filho[3] = criar_quadtree(matriz, x + meio, y + meio, meio, tolerancia);

    return node;
}
