############################################
# Makefile
############################################

# COMPILAÇÃO -> remove objetos, executável && compila o executável 'compressor' usando GTKmm + OpenCV
# make clean && make

# Nome do executável padrão
exe = compressor

# Compilador e flags gerais
CXX        = g++
CXXSTD     = -std=c++17
WARNINGS   = -Wall -Wextra

# path relativo ao include local
LOCAL_INC = -I./include

# pkg-config flags
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4)
OPENCV_LIBS   = $(shell pkg-config --libs   opencv4)
GTKMM_CFLAGS  = $(shell pkg-config --cflags gtkmm-4.0)
GTKMM_LIBS    = $(shell pkg-config --libs   gtkmm-4.0)
SIGC_CFLAGS   = $(shell pkg-config --cflags sigc++-3.0)
SIGC_LIBS     = $(shell pkg-config --libs   sigc++-3.0)

# Flags de compilação e link para "compressor"
CXXFLAGS   = $(CXXSTD) $(WARNINGS) $(GTKMM_CFLAGS) $(SIGC_CFLAGS) $(OPENCV_CFLAGS)
LDFLAGS    = $(GTKMM_LIBS) $(SIGC_LIBS) $(OPENCV_LIBS) -pthread

# Fontes e objetos
SRCS = \
	main.cpp \
	MainWindow.cpp \
	FileItem.cpp \
	CompressorLogic.cpp \
	OutputFolderSelector.cpp \
	StatusBar.cpp \
	QuadtreeCompressor.cpp \
	huffman.cpp \
	bitwrite.cpp \
	bitread.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean

# alvo padrão: compila o "compressor"
all: $(exe)

$(exe): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# regra genérica para .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(exe)
