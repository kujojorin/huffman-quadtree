# huffman-quadtree
Um trabalho de compactação de imagens usando QuadTree e comparando com algoritimo de Huffman para arquivos.

# 1. Pré-processamento

Antes de começar a compressão com QuadTree, a imagem deve estar em um formato adequado (ex: tons de cinza ou cor, matriz quadrada, etc.).

    carregarImagem()
    -> Lê uma imagem do disco e converte para uma matriz (ex: usando OpenCV, PIL ou NumPy).

    converterParaEscalaDeCinza(matrizRGB)
    -> Se necessário, converte a imagem colorida para escala de cinza para simplificar o processo.

    normalizarDimensoes(matriz)
    -> Adapta a matriz para dimensões 2^n x 2^n (caso necessário, preenchendo com zeros).

# 2. Estrutura de Dados

Você precisa definir a estrutura da QuadTree e o nó da árvore.

    struct NohQuadTree
    Representa um nó da árvore. Deve conter:

        -> coordenadas do bloco,

        -> tamanho,

        -> valor médio ou cor média,

        -> flag se é folha ou não,

        -> ponteiros para os 4 filhos (NE, NW, SE, SW).

# 3. Construção da QuadTree (Compressão)

Essas funções constroem a árvore com base em critérios de homogeneidade de blocos.

    ehHomogeneo(matriz, x, y, tamanho, tolerancia)
    -> Verifica se todos os pixels no bloco [x:x+tamanho, y:y+tamanho] estão dentro de uma variação aceitável (tolerancia).

    construirQuadTree(matriz, x, y, tamanho, tolerancia)
    Função recursiva que:

        -> Cria um nó folha se o bloco for homogêneo;

        -> Caso contrário, divide em 4 quadrantes e chama recursivamente.

# 4. Compressão e Armazenamento (opcional)

Para armazenar ou transmitir a árvore:

    serializarQuadTree(nodo)
    -> Converte a árvore para uma representação compacta (ex: lista, string, JSON, binário).

    salvarCompactado(arquivo, dadosSerializados)
    -> Escreve a versão serializada em arquivo.

# 5. Descompressão (Reconstrução da Imagem)

Reconstrói a imagem a partir da árvore:

    desserializarQuadTree(dados)
    -> Constrói novamente a árvore a partir do formato serializado.

    reconstruirImagem(nodo, matriz)
    -> Preenche a matriz de imagem com os valores dos nós folha da árvore.

# 6. Avaliação

Para avaliar a compressão:

    calcularTaxaCompressao(original, compactado)
    -> Mede a relação de tamanho entre original e versão compactada.

    calcularErro(matrizOriginal, matrizReconstruida)
    -> Mede erro médio quadrático (RMSE) ou PSNR.

# Extras (para imagens coloridas)

    construirQuadTreeRGB(matrizRGB, x, y, tamanho, tolerancia)
    -> Extensão da construção da árvore que considera canais RGB individualmente ou juntos.

    ehHomogeneoRGB(...)
    -> Verifica homogeneidade considerando todos os canais de cor.