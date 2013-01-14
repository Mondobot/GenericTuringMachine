#include "parser.h"

std::vector<char> Parser::alfabet;
std::vector<Sym> Parser::simboluri;
std::vector<Set> Parser::sets;
std::vector<std::pair<MT *, std::string> > Parser::known_mt;

bool Parser::will_store = false;
bool Parser::neg_match = false;
Set* Parser::match_set_for_trans = new Set();
std::stack<std::pair<MT *, int> > Parser::working_on;

// Primeste numele unui set deja cunoscut
// Intoarce un pointer catre acel set
// Codul e messy intrucat leg C cu C++ (char * cu std::str mai ales)
Set* Parser::ResolveSet(char *s)
{
	Set *for_matching = NULL;
	for (int i = 0; i < (int)sets.size(); ++i)
		if (strcmp(sets[i].name.c_str(), s) == 0) {
			for_matching = &sets[i];
			break;
		}

	if (for_matching == NULL) {
		// We're fucked!
		std::cout << "Set not found -> " << s << "\n";
		for_matching = new Set();
		for_matching->AddVal('#');
	}

	return for_matching;
}

// Primeste numele unui simbol deja cunoscut
// Intoarce valoarea simbolului
char Parser::ResolveSymbol(char *s)
{
	char val = '#';

	for (int i = 0; i < (int)simboluri.size(); ++i)
		if (strcmp(simboluri[i].name.c_str(), s) == 0) {
			val = simboluri[i].value;
			break;
		}

	return val;
}

// Gets one dirty little string
// Returns a clean and shiny one
char* Parser::str_clean(char *filthy)
{
	return strtok(filthy, " []:=,\\.@");
}

// Adauga un caracter la alfabet
void Parser::add_to_alfabet(char *c)
{
    alfabet.push_back(c[0]);
}

// In functie de "op" adaug numele sau valoarea simbolului
void Parser::add_symbol(char *s, int op)
{
	if (op == SYM_NAME) {
		simboluri.push_back(Sym(str_clean(s)));

	} else if (op == SYM_VAL) {
		simboluri[simboluri.size() - 1].AddVal(s[0]);
	}
}

// In functie de "op" adaug un set nou sau o valoarea
// la un set deja existent(ultimul dintre cele adaugate)
void Parser::add_set(char *s, int op)
{
	if (op == NEW_SET) {
		sets.push_back(Set(str_clean(s)));

	} else if (op == NEW_VAL_TO_SET) {
		sets[sets.size() - 1].AddVal(s[0]);
	}
}

// Adauga o masina Turing noua si o seteaza ca masina
// la care se lucreaza -> working_on.push()
void Parser::NewTM(char *name)
{
	GenericTM *x = new GenericTM();
	std::pair<MT *, std::string> y(x, str_clean(name));
	std::pair<MT *, int> z(x, _MT_);


	known_mt.push_back(y);
	working_on.push(z);
}

// Am terminat de lucrat la masina Turing curenta,
// ne intoarcem cu un nivel mai jos
void Parser::EndNewTM()
{
	working_on.pop();
}

// Adauga un LABEL care nu genereaza salturi
// (util doar ca sa stim unde trebuie sa sarim)
void Parser::AddNoJumpLabel(char *s)
{
	int type = working_on.top().second;

	MT *x = working_on.top().first;
	char *label = Parser::str_clean(s);
	Label *la = new Label(label, NO_JUMP);

	if (type == _MT_) {
		GenericTM *m = dynamic_cast<GenericTM *>(x);

		m->AddLabel(label);
		m->AddMachine(la);
	
	} else if (type == _TRANS_) {
		SingleTransition *tr = dynamic_cast<SingleTransition *>(x);

		tr->AddLabel(label);
		tr->AddMachine(la);
	}
}

// Adauga un LABEL care genereaza salturi
// When LABEL says "Jump!" we beter Jump!
void Parser::AddJumpLabel(char *s)
{
	int type = working_on.top().second;
	MT *x = working_on.top().first;
	char *label = str_clean(s);
	Label *lb = new Label(label + 1, JUMP);

	if (type == _MT_) {
		GenericTM *m = dynamic_cast<GenericTM *>(x);

		m->AddMachine(lb);

	} else if (type == _TRANS_) {
		SingleTransition *tr = dynamic_cast<SingleTransition *>(x);

		tr->AddMachine(lb);
	}
}

// Primeste un token ce reprezinta o masina elementara,
// il parseaza si adauga la masina la care lucram (working_on.top())
// Codul e destul de urat, putea fi evitat printr-o parsare mai
// specifica in Lex, dar era prea mare bataie de cap
// Dis fnctiun gave me cansur
void Parser::AddElementaryTM(char *s)
{
	int type = working_on.top().second;
	MT *x = working_on.top().first;
	MT *to_add;
	char *elem;

	// [LR]|[literal]
	if (strlen(s) == 3) {
		if (s[1] == 'L') {
			to_add = new L();

		} else if (s[1] == 'R') {
			to_add = new R();
		
		} else {
			to_add = new SetChar(s[1]);
		}

	// [[L][!]?(.*)]
	} else if (s[2] == '(') {
		bool direct_match = true;
		bool left = true;

		if (s[1] == 'R')
			left = false;

		if (s[3] == '!')
			direct_match = false;

		elem = strtok(s, "[LR(!)]");

		// Am gasit o variabila!
		if (elem[0] == '&') {
			if (left)
				to_add = new LUntilVar(!direct_match);

			else
				to_add = new RUntilVar(!direct_match);

		} else {
			// Hey, stiu numele asta!
			if (elem[0] == '<') {
				elem = strtok(elem, "<>");
				elem[0] = ResolveSymbol(elem);
			}

			if (left)
				to_add = new LUntil(!direct_match, elem[0]);

			else
				to_add = new RUntil(!direct_match, elem[0]);
		}

	} else {
		elem = strtok(s, "[LR(!)]");

		// Am mai gasit o variabila!
		if (elem[0] == '&') {
			to_add = new SetCharFromVar();

		} else {
			// Stiu numele asta!
			if (elem[0] == '<') {
				elem = strtok(elem, "<>");
				elem[0] = ResolveSymbol(elem);
			}

			to_add = new SetChar(elem[0]);
		}
	}

	// This is the uglies thing I've written all day.
	// Now we (should) have the propper elementary Turing Machine
	// saved in "to_add"
	
	if (type == _MT_) {
		GenericTM *m = dynamic_cast<GenericTM *>(x);

		m->AddMachine(to_add);
	
	} else {
		SingleTransition *tr = dynamic_cast<SingleTransition *>(x);

		tr->AddMachine(to_add);
	}
}

// Primeste numele unei masini (deja) stiute si o adauga
// la masina la care lucram
void Parser::AddGeneralTM(char *s)
{
	int type = working_on.top().second; // Huehuehue, working on top...
	MT *x = working_on.top().first;
	std::string *name = new std::string(str_clean(s));
	MT *to_add = NULL;

	for (int i = 0; i < (int)known_mt.size(); ++i)
		if (name->compare(known_mt[i].second) == 0) {
			to_add = known_mt[i].first;
			break;
		}

	if (to_add == NULL) {
		// I pitty the fool!
		std::cout << "Invalid TM: " << name << "\n";
		return;
	}

	if (type == _MT_) {
		GenericTM *m = dynamic_cast<GenericTM *>(x);

		m->AddMachine(to_add);
	
	} else {
		SingleTransition *tr = dynamic_cast<SingleTransition *>(x);

		tr->AddMachine(to_add);
	}
}

// O Tranzitie multipla (chestia care incapsuleaza
// tranzitii simple, cred ca eu am botezat-o)
void Parser::NewMultiTrans()
{
	MultiTransition *to_add = new MultiTransition();
	MT *x = working_on.top().first;
	std::pair<MT *, int> y(to_add, _M_TRANS_);
	int type = working_on.top().second;

	if (type == _MT_) {
		GenericTM *m = dynamic_cast<GenericTM *>(x);

		m->AddMachine(to_add);
	
	} else {
		SingleTransition *tr = dynamic_cast<SingleTransition *>(x);

		tr->AddMachine(to_add);
	}

	working_on.push(y);
}

// Am terminat cu Masina curenta, facem POP
void Parser::EndNewMultiTrans()
{
	working_on.pop();
}

// Trebuie ca urmatoarea masina / tranzitie sa faca 
// negative_match (!)
void Parser::EnableNegMatch()
{
	Parser::neg_match = true;
}

// Urmatoarea tranzitie o sa salveze match-ul intr-o variabila
void Parser::WillStoreVar()
{
	will_store = true;
}

// Yet another horrible function I've written today
// It recevies a name of a set already stored and
// it creates a new Transition with that set
// It's horribabble
void Parser::AddTransWithSetFromVar(char *s)
{
	s = str_clean(s);
	char *name = strtok(s, "<>");

	Set *match_set = ResolveSet(name);

	SingleTransition *tr = new SingleTransition();

	if (will_store) {
		tr->EnableStoringVar();
		will_store = false;
	}

	tr->AddElementsToMatch(match_set, neg_match);
	neg_match = false;

	// Iterative programming FTW!
	int type = working_on.top().second;
	MT *x = working_on.top().first;

	if (type != _M_TRANS_) {
		// We're fucked (yet again!)
		std::cout << "Working stack is corrupted!\n";
	}

	MultiTransition *mtr;
	mtr = dynamic_cast<MultiTransition *>(x);

	mtr->AddTransition(tr);

	std::pair<MT *, int> y(tr, _TRANS_);
	working_on.push(y);
}

// Adauga un element la un set deja inceput (pentru tranzitii)
void Parser::AddElemToMatchSet(char *s)
{
	char *elem = str_clean(s);

	if (elem[0] == '<') {
		elem = strtok(elem, "<>");
		elem[0] = ResolveSymbol(elem);
	}

	match_set_for_trans->AddVal(elem[0]);
}

// Adauga tranzitia cu setul pe care tocmai l-am citit
void Parser::AddTransWithSet()
{
	SingleTransition *tr = new SingleTransition();

	if (will_store) {
		tr->EnableStoringVar();
		will_store = false;
	}

	tr->AddElementsToMatch(match_set_for_trans, neg_match);
	neg_match = false;

	int type = working_on.top().second;
	MT *x = working_on.top().first;
	if (type != _M_TRANS_) {
		// We're fucked (yet again!)
		std::cout << "Working stack is corrupted!\n";
	}

	MultiTransition *mtr = dynamic_cast<MultiTransition *>(x);
	mtr->AddTransition(tr);

	std::pair<MT *, int> y(tr, _TRANS_);
	working_on.push(y);
	match_set_for_trans = new Set();
}

// Ruleaza masina primita ca input (rezolva tema practic :P)
void Parser::Run(std::string name, char *tape)
{
	Tape::SetToStr(tape);
	bool found = false;

	int i;
	for (i = 0; i < (int)known_mt.size(); ++i)
		if (name.compare(known_mt[i].second) == 0) {
			known_mt[i].first->Run();
			found = true;
		}

	if (!found) {
		std::cout << "TM not found! -> " << name << "\n";
	}

	Tape::Print();
}
