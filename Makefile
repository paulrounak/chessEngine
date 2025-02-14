CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRC = src/attack_detection.cpp src/board_representation.cpp src/engine.cpp src/fen_parser.cpp \
      src/legal_moves.cpp src/move_executor.cpp src/move_generator.cpp src/perft_test_driver.cpp \
      src/utils.cpp src/eval.cpp src/search.cpp

OBJDIR = build
OBJ = $(SRC:src/%.cpp=$(OBJDIR)/%.o)

OUT = chessEngine

.PHONY: all clean

all: $(OUT)

$(OUT): $(OBJ)
	$(CXX) $(OBJ) -o $(OUT)

$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(OUT)
