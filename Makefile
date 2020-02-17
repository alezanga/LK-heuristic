CC = g++-7
CPPFLAGS = -Wall -O3 -std=c++17
LDFLAGS =

CPX_BASE    = /opt/ibm/ILOG/CPLEX_Studio128
CPX_INCDIR  = $(CPX_BASE)/cplex/include
CPX_LIBDIR  = $(CPX_BASE)/cplex/lib/x86-64_linux/static_pic
CPX_LDFLAGS = -lcplex -lm -pthread -ldl -lstdc++fs
PY_CFLAGS  	= $(shell python-config --cflags)
PY_LDFLAGS	= $(shell python-config --ldflags)
ODIR = build
SRCDIR = src
BIN = bin

$(shell mkdir -p $(ODIR) $(BIN))

OBJ = $(ODIR)/CPLEX.o $(ODIR)/TSPsolution.o $(ODIR)/TSPinstance.o \
	$(ODIR)/Tour.o $(ODIR)/LK.o $(ODIR)/utilities.o $(ODIR)/Pair.o

$(ODIR)/%.o: $(SRCDIR)/%.cpp
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c $^ -o $@

MAIN = $(ODIR)/main.o $(OBJ)
main: $(MAIN)
		$(CC) $(PY_CFLAGS) $(CPPFLAGS) $(MAIN) -o $(BIN)/main -L$(CPX_LIBDIR) $(CPX_LDFLAGS) $(PY_LDFLAGS)

CAL = $(ODIR)/calibrate.o $(OBJ)
calibrate: $(CAL)
		$(CC) $(CPPFLAGS) $(CAL) -o $(BIN)/calibrate -L$(CPX_LIBDIR) $(CPX_LDFLAGS)

TEST = $(ODIR)/test.o $(OBJ)
test: $(TEST)
		$(CC) $(CPPFLAGS) $(TEST) -o $(BIN)/test -L$(CPX_LIBDIR) $(CPX_LDFLAGS)

OTHERS = $(ODIR)/test.o $(ODIR)/calibrate.o $(ODIR)/main.o $(BIN)/main $(BIN)/calibrate $(BIN)/test
clean:
		rm -rf $(OBJ) $(OTHERS) $(SRCDIR)/*.pyc $(SRCDIR)/__pycache__

.PHONY: clean