#ifndef _ELEMENTARY_H_
#define _ELEMENTARY_H_

#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>

#include "aux.h"

#define IS_NOT false
#define IS true

// Tatal tuturor masinilor Turing
class MT {
	public:
		virtual void Run()=0;
		virtual bool HasName()=0;
};

// Rulata, muta cursorul la stanga
class L : public MT {
	public:
		L() {};
		~L() {};

		virtual void Run() {
			Tape::L();
		}

		virtual bool HasName() {
			return false;
		}
};

// Rulata, muta cursorul la dreapta
class R : public MT {
	public:
		R() {};
		~R() {};

		virtual void Run() {
			Tape::R();
		}

		virtual bool HasName() {
			return false;
		}
};

// La rulare pune un caracter pe banda
class SetChar : public MT {
	private:
		char x;

	public:
		SetChar(char x) : x(x) {};
		~SetChar() {};

		virtual void Run() {
			Tape::SetChar(x);
		}

		virtual bool HasName() {
			return false;
		}
};

// La rulare pune pe banda caracterul din varful stivei de variabile
class SetCharFromVar : public MT {
	public:
		SetCharFromVar() {};
		~SetCharFromVar() {};

		virtual void Run() {
			Tape::SetChar(Env::stack_var.top());
		}

		virtual bool HasName() {
			return false;
		}
};

// Merge stanga pana cand da de caracterul pe care il cauta
class LUntil : public MT {
	private:
		char x;
		bool neg_match;
	
	public:
		LUntil(bool neg_match, char until_what) : neg_match(neg_match), x(until_what) {};
		~LUntil() {};

		virtual void Run() {
			Tape::L();

			while ((!neg_match && Tape::ElementAtCursor() != x) ||
					(neg_match && Tape::ElementAtCursor() == x)) {
				Tape::L();
			}
		}

		virtual bool HasName() {
			return false;
		}
};

// Merge dreapta pana cand da de caracterul pe care il cauta
class RUntil : public MT {
	private:
		char x;
		bool neg_match;

	public:
		RUntil(bool neg_match, char until_what) : neg_match(neg_match), x(until_what) {};
		~RUntil() {};

		virtual void Run() {
			Tape::R();

			while ((!neg_match && Tape::ElementAtCursor() != x) ||
					(neg_match && Tape::ElementAtCursor() == x)) {
				Tape::R();
			}
		}

		virtual bool HasName() {
			return false;
		}
};

// Merge stanga pana cand da de caracterul care se afla in varful stivei de
// variabile
class LUntilVar : public MT {
	private:
		bool neg_match;
	
	public:
		LUntilVar(bool neg_match) : neg_match(neg_match) {};
		~LUntilVar() {};

		virtual void Run() {
			Tape::L();

			while ((!neg_match && Tape::ElementAtCursor() != Env::stack_var.top()) ||
					(neg_match && Tape::ElementAtCursor() == Env::stack_var.top()))
				Tape::L();
		}

		virtual bool HasName() {
			return false;
		}
};

// Merge drepta pana cand da de caracterul care se afla in varful stivei de
// variabile
class RUntilVar : public MT {
	private:
		bool neg_match;

	public:
		RUntilVar(bool neg_match) : neg_match(neg_match) {};
		~RUntilVar() {};

		virtual void Run() {
			Tape::R();

			if (Env::stack_var.empty()) {
				std::cout << "Stack Empty!!\n";
				return;
			}

			while ((!neg_match && Tape::ElementAtCursor() != Env::stack_var.top()) ||
					(neg_match && Tape::ElementAtCursor() == Env::stack_var.top())) {
				Tape::R();
			}
		}

		virtual bool HasName() {
			return false;
		}
};

// Am facut din etichete masini turing (sper sa nu fiu linsat)
// Sunt 2 tipuri de Label in functie de campul jump
// Daca jump == true provoaca saltul cand este rulata, altfel il opreste
class Label : public MT {
	public:
		std::string name;
		bool jump;

		Label() : name("NAN"), jump(false) {};
		Label(std::string name, bool should_I) : name(name), jump(should_I) {};
		~Label() {};

		virtual void Run() {
			//std::cout << "Am dat de " << name << std::endl;

			if (jump) {
				Env::current_label = name;
				Env::jump = true;

			} else {
				Env::current_label = "__";	// current_label is bored, nothing
				Env::jump = false;			// interesting is going on ...
			}

		}

		virtual bool HasName() {
			return true;
		}
};
#endif
