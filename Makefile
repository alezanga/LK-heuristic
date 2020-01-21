CC = g++-7
CPPFLAGS = -g -Wall -O0
LDFLAGS =

CPX_BASE    = /opt/ibm/ILOG/CPLEX_Studio128
CPX_INCDIR  = $(CPX_BASE)/cplex/include
CPX_LIBDIR  = $(CPX_BASE)/cplex/lib/x86-64_linux/static_pic
CPX_LDFLAGS = -lcplex -lm -pthread -ldl -lstdc++fs
ODIR = build
SRCDIR = src
BIN = bin

OBJ = $(ODIR)/main.o $(ODIR)/TSPmodel.o $(ODIR)/TSPsolution.o $(ODIR)/CostGen.o \
	$(ODIR)/Tour.o $(ODIR)/LK.o $(ODIR)/IteratedLK.o $(ODIR)/Params.o $(ODIR)/datatypes.o

$(ODIR)/%.o: $(SRCDIR)/%.cpp
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c $^ -o $@

main: $(OBJ)
		$(CC) $(CPPFLAGS) $(OBJ) -o $(BIN)/main -L$(CPX_LIBDIR) $(CPX_LDFLAGS)

clean:
		rm -rf $(OBJ) $(BIN)/main

.PHONY: clean

$(shell mkdir -p $(ODIR) $(SRCDIR))
