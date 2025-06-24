#include <iostream>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <json.hpp>

using json = nlohmann::json;
using namespace std;

//prototipos
int carregarImagem(const string &file);
//FILE* fopen_e_teste( const char *caminho, const char* modo);
int aproximacao(int numero);
vector<vector<unsigned char>> normalizar_matriz(vector<vector<unsigned char>> matriz);
struct no{
    int x,y;
    int tamanho;
    char cor; 
    bool folha;   
    no* filho[4];
    no(){x = 0;y = 0;folha = false;tamanho = 0;
          for(int i = 0; i< 4; i++) filho[i] = nullptr; }
};
bool homegenea(vector<vector<unsigned char>> matriz,int x,int y,int tamanho,int tolerancia);
no *criar_quadtree(const vector<vector<unsigned char>> &matriz, int x,int y,int tamanho,int tolerancia);


int main()
{
    int escolha;
    string nomeArquivo;
    FILE* f;

    do
    {
        // Menu de opções
        cout << "Menu de Opções:\n";
        cout << "1. Compactar arquivo de imagem\n";
        cout << "2. Compactar arquivo texto\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opção: ";
        cin >> escolha;

        // Estrutura switch para verificar a escolha
        switch (escolha)
        {
        case 1:
            
            cout << "Você escolheu a Opção 1\n";
            cout << "Compactacao de arquivo imagem" << endl;

            cout << "Informe o nome do arquivo a ser compactado " << endl;
            cin >> nomeArquivo;

            if(carregarImagem(nomeArquivo) != 0){
                cout << "Imagem nao foi carregada, tente novamente" << endl;
            }

            break;
        case 2:
            cout << "Você escolheu a Opção 2\n";
            cout << "Compatacao de arquivo texto" << endl;

            cout << "Informe o nome do arquivo a ser compactado" << endl;
            cin >> nomeArquivo;

            //.c_str converte string para const char 
         //   f = fopen_e_teste(nomeArquivo.c_str(), "r");




            break;
        case 0:
            cout << "Saindo do programa.\n";
            break;
        default:
            cout << "Opção inválida. Tente novamente.\n";
        }
    } while (escolha != 0);

    cout << "Programa encerrado.\n";
    return 0;
}

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

int carregarImagem(const string &file)
{

    // funcao mat da biblioteca opencv
    cv::Mat imagem = cv::imread(file);

    if (imagem.empty())
    {
        cerr << "Erro ao carregar imagem." << endl;
        return -1;
    }

    cout << "Imagem carregada com sucesso" << endl;
    // exibir a imagem
    cv::imshow("", imagem);
    //espera o usuario apertar alguma tecla
    cv::waitKey(0);
    //fecha a janela do opencv
    cv::destroyAllWindows();

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

    auto quadrada = normalizar_matriz(mat);
    

    cout << "Imagem carregada para cinza com sucesso" << endl;
    // exibir a imagem
    cv::imshow("", imagem);
    //espera o usuario apertar alguma tecla
    cv::waitKey(0);
    //fecha a janela do opencv
    cv::destroyAllWindows();

    return 0;
}
//definação:verifica se uma parte da matriz tem valores proximos,definindo a aceitabililidade de diferença usando a tolerancia
//requer: uma matriz,um ponto,o tamanho do quadrado que se deseja verificar,e o numero que esse bloco pode der de valores diferentes entre si;
//retorna: se a matriz é homegenea ou não segundo a tolerancia;

bool homegenea(vector<vector<unsigned char>> matriz,int x,int y,int tamanho,int tolerancia){
     unsigned char menor =225;
     unsigned char maior = 0;
   //for vai de x a x0 e y a y0,ver se a faixa de numeros é menor que a tolerancia;
     for (int i = x;i < x + tamanho; i++)
         for(int j = y; j < y + tamanho; j++){
           unsigned char valor = matriz [i][j];
            menor = min(menor,valor);
            maior = max(maior,valor);

            if(maior - menor > tolerancia )
                return false;
         }
    return true;
}

no *criar_quadtree(const vector<vector<unsigned char>> &matriz, int x,int y,int tamanho,int tolerancia){
     no *node = new no();
         
     if(homegenea (matriz, x, y, tamanho, tolerancia)){
        node->folha = true;
        node->cor = matriz[x][y];
        node->tamanho =tamanho;
        return node;
     }

     int meio = tamanho / 2;

     node->filho[0] = criar_quadtree(matriz, x, y, meio, tolerancia);
     node->filho[1] = criar_quadtree(matriz, x, y +meio, meio, tolerancia);
     node->filho[2] = criar_quadtree(matriz, x + meio, y, meio, tolerancia);
     node->filho[3] = criar_quadtree(matriz, x + meio, y + meio, meio, tolerancia);

     return node;
}

json converter (no* quadtree){
  json j;
  if(quadtree->folha){
   j["folha"] = true;
   j["valor"] =quadtree->cor;
  }
  else{
    j["folha"] = false;
    j["filhos"] =json::array();
    
  }
}