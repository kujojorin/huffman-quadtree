#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;
int aproximacao(int numero);
vector<vector<unsigned char>> normalizar_matriz(vector<vector<unsigned char>> matriz);

int main()
{
    int escolha;
    string nomeArquivo;

    do
    {
        // Menu de opções
        cout << "Menu de Opções:\n";
        cout << "1. Compactar arquivo de imagem\n";
        cout << "2. Compactar arquivo texto\n";
        cout << "3. Opção 3\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opção: ";
        cin >> escolha;
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
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
            // Coloque aqui o código a ser executado para a Opção 2
            break;
        case 3:
            cout << "Você escolheu a Opção 3\n";
            // Coloque aqui o código a ser executado para a Opção 3
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
    //fecha a janela do opencv
    cv::destroyAllWindows();

    return 0;
}