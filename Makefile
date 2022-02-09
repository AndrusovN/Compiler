OBJ = lex.yy.o grammar.tab.o main.o
PROG = intellectual-calc
LIBS =
CXXFLAGS +=-Wall
LEXFILE = lexer.ll
LEX = flex

$(PROG): $(OBJ)
	$(CXX) -o $(PROG) $(OBJ) $(LIBS) $(CXXFLAGS)

lex.yy.cc: $(LEXFILE) grammar.tab.cc
	flex -o lex.yy.cc $(LEXFILE)

grammar.tab.cc: grammar.yy
	bison -d grammar.yy -Wall -v

depend:
	$(CXX) -MM $(OBJ:.o=.cc) > .depend
-include .depend

clean:
	rm -f *.o lex.yy.cc grammar.tab.cc
