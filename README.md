# Compressor de Imagens e Textos com Quadtree e Huffman

## Sobre o Projeto

Este projeto é uma ferramenta de compressão e descompressão de arquivos desenvolvida em C++, utilizando o framework **GTKmm 4** para a interface gráfica. O programa oferece dois algoritmos distintos: **Huffman** para arquivos de texto e **Quadtree** para imagens, permitindo ao usuário visualizar o progresso e os resultados de forma clara e interativa.

## Funcionalidades

-   **Interface Gráfica Intuitiva:** Construído com GTKmm 4, o programa oferece uma janela única com todos os controles necessários para a operação.
-   **Dois Algoritmos de Compressão:**
    -   **Huffman:** Otimizado para a compressão eficiente de arquivos de texto (`.txt`).
    -   **Quadtree:** Um algoritmo visual para a compressão de imagens (`.png`, `.jpg`), onde a qualidade pode ser ajustada.
-   **Modo Duplo:** Suporta tanto **compressão** quanto **descompressão** para os algoritmos implementados.
-   **Seleção de Arquivos Inteligente:** O seletor de arquivos adapta os filtros automaticamente com base no algoritmo e no modo (compressão/descompressão) selecionado.
-   **Controle de Qualidade (Quadtree):** Uma barra de rolagem permite ao usuário ajustar o nível de qualidade da compressão de imagens, balanceando o tamanho do arquivo final e a fidelidade visual.
-   **Feedback em Tempo Real:**
    -   Uma barra de progresso na interface gráfica e no terminal exibe o andamento do processo.
    -   A barra de status informa sobre ações, erros e sucesso.
-   **Relatório de Resultados:** Ao final de cada operação, um diálogo modal exibe um resumo com o tamanho inicial, tamanho final e a taxa de compressão/descompressão.
-   **Atalho para Pasta de Saída:** Após uma operação bem-sucedida, o explorador de arquivos do sistema é aberto automaticamente na pasta de destino.
-   **Tratamento de Erros Robusto:** O programa é projetado para capturar erros (como tentar descomprimir um arquivo corrompido) e informar o usuário através de um diálogo, em vez de fechar inesperadamente.

## Como Funciona

### Compressão Huffman
A codificação de Huffman é um algoritmo de compressão sem perdas (lossless). Ele funciona analisando a frequência de cada caractere no arquivo de texto. Caracteres que aparecem com mais frequência (como 'a' e 'e' em português) recebem códigos binários mais curtos, enquanto caracteres raros recebem códigos mais longos. O resultado é um arquivo binário (`.huf`) que representa os mesmos dados de forma mais compacta.

### Compressão Quadtree
A Quadtree é um algoritmo de compressão com perdas (lossy), ideal para imagens. A lógica é visual e recursiva:
1.  A imagem é dividida em quatro quadrantes iguais.
2.  Para cada quadrante, o algoritmo verifica se as cores dentro dele são "parecidas" o suficiente, com base em um valor de **tolerância** definido pelo usuário.
3.  Se as cores forem parecidas, o quadrante inteiro é representado por uma única cor média (ele vira uma "folha" da árvore).
4.  Se as cores variarem muito (acima da tolerância), o quadrante é novamente dividido em quatro, e o processo se repete para cada um dos novos quadrantes.
O resultado é salvo em um arquivo `.json` que descreve essa árvore de quadrantes.

## Como Executar

### Pré-requisitos e Compilação

O código-fonte é multiplataforma, mas o processo de compilação depende do seu sistema operacional.

---

#### **Opção 1: Linux (Ambiente Recomendado)**

**1. Instalação das Dependências (Para sistemas baseados em Debian/Ubuntu):**

Abra um terminal e execute o seguinte comando para instalar o compilador, o `make` e as bibliotecas GTKmm e OpenCV:

```bash
sudo apt-get update && sudo apt-get install build-essential make libgtkmm-4.0-dev libopencv-dev
```

**2. Compilação:**

Com todas as dependências instaladas, navegue até a pasta raiz do projeto no terminal e simplesmente execute o comando `make`:

```bash
make
```

Isso irá compilar todos os arquivos-fonte e gerar um executável chamado `compressor`.

---

#### **Opção 2: Windows (Compilação Avançada)**

Compilar no Windows requer a configuração de um ambiente de desenvolvimento similar ao do Linux. A forma mais comum é utilizando o **MSYS2**.

**1. Instalação do Ambiente MSYS2:**

-   Acesse o site [msys2.org](https://www.msys2.org/) e siga as instruções para instalar o MSYS2.
-   Após a instalação, abra o terminal **MSYS2 MINGW64** (importante usar a versão de 64 bits).

**2. Instalação das Dependências via `pacman`:**

No terminal MSYS2 MINGW64, execute os seguintes comandos para instalar o compilador e as bibliotecas necessárias:

```bash
# Atualizar o gerenciador de pacotes
pacman -Syu

# Instalar as ferramentas de compilação
pacman -S --needed base-devel mingw-w64-x86_64-toolchain

# Instalar GTKmm 4 e OpenCV
pacman -S mingw-w64-x86_64-gtkmm4 mingw-w64-x86_64-opencv
```

**3. Compilação:**

O `makefile` fornecido no projeto foi escrito para Linux. Ele pode precisar de pequenos ajustes para funcionar corretamente no ambiente MSYS2, mas a estrutura básica é a mesma. Navegue até a pasta do projeto dentro do terminal MSYS2 MINGW64 e execute:

```bash
make
```

Isso deverá gerar o arquivo `compressor.exe`.

---

### Execução

-   **No Linux:**
    ```bash
    ./compressor
    ```
-   **No Windows (dentro do terminal MSYS2):**
    ```bash
    ./compressor.exe
    ```

## Uso da Interface Gráfica

A interface foi projetada para ser um fluxo de trabalho de cima para baixo:

1.  **Escolha o algoritmo:** Selecione "Huffman (Texto)" ou "Quadtree (Imagem)".
2.  **Modo de Operação:** Marque a caixa "Modo Descompressão" se desejar descomprimir um arquivo.
3.  **Adicionar Arquivos:** Clique no botão. O seletor de arquivos já estará filtrando pelo tipo de arquivo correto.
4.  **Ajustar Qualidade (para Quadtree):** Se estiver comprimindo uma imagem, use a barra "Qualidade da Compressão". **Valores altos (próximos de 100) significam alta qualidade** e menor compressão.
5.  **Selecionar Pasta de Saída:** Escolha onde os arquivos resultantes serão salvos.
6.  **Iniciar Processo:** Clique em "Iniciar Compressão" (ou "Descompressão").
7.  **Verificar Resultado:** Um diálogo aparecerá com o resumo da operação. Se a operação for bem-sucedida, a pasta de saída será aberta automaticamente.