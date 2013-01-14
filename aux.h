#ifndef _AUX_H_
#define _AUX_H_

#include <list>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stack>

// Clasa simbol (mai degraba o strucutra de C cu pretentii)
class Sym {
	public:
		std::string name;
		char value;

		Sym() : name("undef"), value(0) {};
		Sym(char *name) : name(name) {};
		~Sym() {};

		void AddVal(char val) {
			value = val;
		}
};

// Clasa Set (tot o structura cu pretentii)
// Tine un vectori de valori asociate cu un nume
class Set {
	public:
		std::string name;
		std::vector<char> values;

		Set() : name("empty") {};
		Set(char *name) : name(name) {};
		~Set() {};

		void AddVal(char val) {
			values.push_back(val);
		}

		bool IsMember(char val) {
			for (int i = 0; i < (int)values.size(); ++i) {

				if (values[i] == val) {
					return true;
				}
			}

			return false;
		}

		bool IsEmpty() {
			return (values.size() == 0) ? true : false;
		}
};

// Banda unei masini turing.
// Stie sa mearga stanga, dreapta, sa puna un caracter
// si sa se seteze dupa un anumit string
// De asemenea se poate si afisa
class Tape {	// (duct) tape can turn any "no!" into "mmmph!"
	public:
		static std::list<char> *tape;
		static std::list<char>::iterator cursor;
		static int curs_pos;

		Tape();
		~Tape();

		static void L();
		static void R();
		static void SetChar(char x);
		static void SetToStr(char *s);
		static char ElementAtCursor();
		static void Print();

	private:
		static void UpdateIter();
		static void CleanTape();
};

// Clasa Envirnoment unde se tin niste variabile globale
// pe care le folosesc celalalte clase
class Env {
	public:
		static std::string current_label;		// Eticheta la care trebuie
										// sa sarim

		static std::stack<char> stack_var;		// Stiva de variabile
										// Fiecare save ajunge aici

		static bool jump;					// Devine true cand trebuie
										// sa sarim la o eticheta
};
#endif
