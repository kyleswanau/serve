CC  = gcc
EXE = serve
SRC = source/main.c

# BUILD TARGETS
$(EXE): $(SRC)
	$(CC) $^ -o $@

# UTILITY TARGETS
run: $(EXE)
	./$(EXE)

clean:
	rm $(EXE)
