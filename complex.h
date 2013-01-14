#ifndef _COMPLEX_H_
#define _COMPLEX_H_

#include <stdlib.h>
#include <vector>

#include "aux.h"
#include "elementary.h"

// O tranzitie singulara (adica {ceva} -> altceva)
// Mai multe sunt grupata intr-o MultiTransition
// Cand este rulata, ruleaza la randul ei toate masinile din seq
class SingleTransition : public MT {
	private:
		bool neg_match;
		bool storing;
	
		int cursor;
		Set *to_match;
		std::vector<std::string> labels;
		std::vector<MT *> seq;

	public:
		SingleTransition() : cursor(-1), neg_match(true), storing(false) {};
		~SingleTransition() {};

		void EnableStoringVar();
		void AddElementsToMatch(Set *x, bool neg_match);
		void AddMachine(MT *x);
		void AddLabel(std::string label);
		bool IsThisMyLabel(std::string label);
		void JumpToLabel(std::string label);
		bool FindMatch(char x);

		virtual void Run();

		virtual bool HasName() {
			return false;
		}
};

// O tranzitie multipla (de fapt tine un vector de tranzitii singulare)
// La rulare testeaza care dintre tranzitiile singulare face match
// si o ruleaza pe aceea
class MultiTransition : public MT {
	private:
		int cursor;
		std::vector<SingleTransition *> seq;

	public:
		MultiTransition() : cursor(-1) {};
		~MultiTransition();

		void AddTransition(SingleTransition *tr);
		virtual void Run();

		virtual bool HasName() {
			return false;
		}
};
#endif
