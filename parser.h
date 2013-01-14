#ifndef _ENV_H_
#define _ENV_H_

#include <string.h>
#include <utility>

#include "aux.h"
#include "elementary.h"
#include "complex.h"
#include "generic.h"

#define SYM_NAME 0
#define SYM_VAL 1
#define NEW_SET 2
#define NEW_VAL_TO_SET 3
#define SEPARATOR '#'

#define _MT_ 0
#define _TRANS_ 1
#define _M_TRANS_ 2

#define JUMP true
#define NO_JUMP false

// Clasa Parser se ocupa de crearea tuturor
// masinilor, simbolurilor, seturilor etc din
// tokeni primiti de la FLEX
class Parser {
	public:
		static std::vector<char> alfabet;
		static std::vector<Sym> simboluri;
		static std::vector<Set> sets;
		static std::vector<std::pair<MT *, std::string> > known_mt;

		// Intalnirea anumitor tipuri de tag-uri inaintea
		// folosirii lor ma obliga sa tin niste flag-uri
		static bool will_store;
		static bool neg_match;
		static Set *match_set_for_trans;
		static std::string current_label;
		static std::stack<std::pair<MT *, int> > working_on;

		Parser() {};
		~Parser() {};

		// Partea de parsare dinaintea masinilor turing
		// Initial erau in C asa ca am preferat sa le las
		// denumirile de C (No CamelCase)
		static void add_to_alfabet(char *c);
		static void add_symbol(char *s, int op);
		static void add_set(char *s, int op);

		static void NewTM(char *s);
		static void EndNewTM();

		static void AddNoJumpLabel(char *s);
		static void AddJumpLabel(char *s);

		static void AddElementaryTM(char *s);
		static void AddGeneralTM(char *s);

		static void NewMultiTrans();
		static void EndNewMultiTrans();

		static void EnableNegMatch();
		static void WillStoreVar();
		static void AddTransWithSetFromVar(char *s);
		static void AddTransWithSet();
		static void AddElemToMatchSet(char *s);

		// And Finally!
		static void Run(std::string name, char *tape);

		// Functii helper (lucreaza doar intern pe clasa Parser)
		static Set* ResolveSet(char *s);
		static char ResolveSymbol(char *s);
		static char* str_clean(char *filthy);
};
#endif
