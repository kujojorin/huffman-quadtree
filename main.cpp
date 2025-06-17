#include <iostream>

using namespace std;

int main()
{
    int escolha;

    do
    {
        // Menu de opções
        cout << "Menu de Opções:\n";
        cout << "1. Opção 1\n";
        cout << "2. Opção 2\n";
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
            // Coloque aqui o código a ser executado para a Opção 1
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