#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

#include "huffman.h"
#include"quadtree_des.h"
#include"quadtree_com.h"

#include <opencv2/opencv.hpp>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// prototipos
FILE* fopen_e_teste( const char *caminho, const char* modo);

int main()
{
    int escolha;
    string nomeArquivo;
    string nomeArquivoSaida = "compacto.huf"; // nome pre definido 
    string nomeArquivoDescompactado = "descompacto.txt";
    huffman hf;
    FILE *f;

    do
    {
        // Menu de opções
        cout << "Menu de Opções:\n";
        cout << "1. Compactar arquivo de imagem\n";
        cout << "2. Compactar arquivo texto\n";
        cout << "3. Descompactar arquivo de imagem\n";
        cout << "4. Descompactar arquivo texto\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opção: ";
        cin >> escolha;

        // Estrutura switch para verificar a escolha
        switch (escolha)
        {
        case 1:{
            int tamanho;
            cout << "Você escolheu a Opção 1\n";
            cout << "Compactacao de arquivo imagem" << endl;

            cout << "Informe o nome do arquivo a ser compactado " << endl;
            cin >> nomeArquivo;

            vector<vector<unsigned char>>imagem = carregarImagem(nomeArquivo,tamanho);
            no *raiz = criar_quadtree(imagem, 0, 0,tamanho,30);
             cout << "2: Quadtree criada" << endl;
            json resultado = converter(raiz);
             cout << "3: JSON convertido" << endl;  

            ofstream arquivo("compacto.json");
            arquivo << resultado.dump(4);
            arquivo.close();
            
            break;
        }
        case 2:
            cout << "Você escolheu a Opção 2\n";
            cout << "Compatacao de arquivo texto" << endl;

            cout << "Informe o nome do arquivo a ser compactado" << endl;
            cin >> nomeArquivo;

            hf.compressao(nomeArquivo, nomeArquivoSaida);

           
            break;
        case 0:
            cout << "Saindo do programa.\n";
            break;
        case 3:{ 
            json arquivo;
            cout << "Informe o nome do arquivo comprimido: ";
            cin  >> nomeArquivo;
            std::ifstream entrada2(nomeArquivo);
            entrada2 >> arquivo;
            entrada2.close();
       
    
        }    
        case 4:
            cout << "Informe o nome do arquivo comprimido: ";
            cin  >> nomeArquivo;
            hf.expandir(nomeArquivo, nomeArquivoDescompactado);
        break;
        default:
            cout << "Opção inválida. Tente novamente.\n";
        }
    } while (escolha != 0);

    cout << "Programa encerrado.\n";
    return 0;
}


FILE *fopen_e_teste(const char *caminho, const char *modo)
{
    FILE *f;
    f = fopen(caminho, modo);
    if (f == NULL)
    {
        perror("Erro ao tentar abrir o arquivo.\n");
        exit(1);
    }
    return f;
}

// implementacao do huffman
