#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aux.h"

std::list<char> * Tape::tape = new std::list<char>(3, '#');
std::list<char>::iterator Tape::cursor = ++tape->begin();
int Tape::curs_pos = 1;

Tape::Tape() {}
Tape::~Tape() {}

// Muta banda cu o pozitie la Stanga
void Tape::L()
{
	// Banda e infinita la stanga
	if (curs_pos == 1) {
		tape->push_front('#');
		UpdateIter();

	} else {
		--cursor;
		--curs_pos;
	}
	//Tape::Print();
}

// Multa banda cu o pozitie la Dreapta
void Tape::R()
{
	// Banda e infinita si la dreapa (sa nu se supere)
	if (curs_pos == tape->size() - 2) {
		tape->push_back('#');
		++curs_pos;
		UpdateIter();
	
	} else {
		++cursor;
		++curs_pos;
	}
}

// Pune un caracter la pozitia curenta
void Tape::SetChar(char x)
{
	*cursor = x;
	UpdateIter();
}

// Copiaza un string pe banda
void Tape::SetToStr(char *s)
{

	int keep_curs_at = 1;

	for (int i = 0; i < strlen(s); ++i) {
		if (s[i] == '>') {
			keep_curs_at = i + 1;

		} else {
			SetChar(s[i]);
			R();
		}
	}

	curs_pos = keep_curs_at;
	UpdateIter();
}

// Intoarce elementul la care este cursorul
char Tape::ElementAtCursor()
{
	return *cursor;
}

// Afiseaza continutul benzii
void Tape::Print()
{
	// Curatam banda de extra '#'
	CleanTape();
	
	int i = 0;
	for (std::list<char>::iterator x = tape->begin(); x != tape->end(); ++x, ++i) {
		if (i == curs_pos)
			printf(">");

		printf("%c", *x);
	}

	printf("\n");
}

// Iteratorul trebuie sa fie consistent asa ca avem grija sa fie mereu valid
void Tape::UpdateIter()
{
	if (curs_pos > tape->size()) {	// Nasol moment
		printf("Invalid tape \n");
		exit(1);
	}

	cursor = tape->begin();
	for (int i = 0; i < curs_pos; ++i)
		++cursor;
}

// Curata Banda de extra '#'
void Tape::CleanTape()
{
	std::list<char>::iterator x = tape->begin();

	for (int i = 0; i < (int)tape->size() - 1; ++i) {
		if (*x != '#' || i == curs_pos)
			break;

		if (*x == '#' && i > 0) {
			tape->pop_front();
			--curs_pos;
			--i;
		}

		++x;
	}

	x = tape->end();
	--x;
	for (int i = (int)tape->size() - 1; i >= 0; --i) {
		if (*x != '#')
			break;
		
		if (i == curs_pos) {
			tape->pop_back();
			break;
		}

		if (*x == '#' && i < (int)tape->size() - 1)
			tape->pop_back();

		--x;
	}
}

bool Env::jump = false;
std::string Env::current_label = "__";
std::stack<char> Env::stack_var;
