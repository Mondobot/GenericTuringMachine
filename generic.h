#ifndef _GENERIC_H_
#define _GENERIC_H_

#include "aux.h"
#include "elementary.h"
#include "complex.h"

// Masina Turing generica, apogeul culturii si tehnicii
class GenericTM : public MT {
	private:
		int cursor;
		std::vector<std::string> labels;
		std::vector<MT *> seq;

	public:
		GenericTM() : cursor(-1) {};
		~GenericTM() {};

		void AddMachine(MT *x);
		void AddLabel(std::string label);
		bool IsThisMyLabel(std::string label);
		void JumpToLabel(std::string label);

		virtual void Run();

		virtual bool HasName() {
			return false;
		}
};
#endif
