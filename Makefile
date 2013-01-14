all:
	flex parser.lex
	g++ -g lex.yy.c aux.cpp parser.cpp complex.cpp generic.cpp -lfl -o mtx

clean:
	rm mtx
