#include "complex.h"

// Aceasta tranzitie va pune pe stiva o variabila
void SingleTransition::EnableStoringVar()
{
	storing = true;
}

// Salveaza elementele pentru care ar trebui sa faca match
void SingleTransition::AddElementsToMatch(Set *x, bool ceva)
{
	to_match = x;
	neg_match = ceva;
}

// Adauga o masina la secventa ei de masini ce trebuie rulate in caz
// de match
void SingleTransition::AddMachine(MT *x)
{
	seq.push_back(x);
	cursor = 0;
}

// Daca exista un NoJumpLabel la nivelul lui
// el trebuie sa stie pentru a prinde un eventual
// jump
void SingleTransition::AddLabel(std::string label)
{
	if (!IsThisMyLabel(label))
		labels.push_back(label);
}

// This comment is self explanatory
bool SingleTransition::IsThisMyLabel(std::string label)
{
	bool found = false;

	for (int i = 0; i < (int)labels.size(); ++i)
		if (labels[i].compare(label) == 0) {
			found = true;
			break;
		}

	return found;
}

// In caz ca avem Labelul la care trebuie sa se sara, Sarim!
void SingleTransition::JumpToLabel(std::string label)
{
	if (labels.size() == 0) {
		std::cout << "Nu exista \"" << label << "\", ne-am futut\n";
		return;
	}

	for (int i = 0; i < (int)seq.size(); ++i)
		if (seq[i]->HasName() && 
			(dynamic_cast<Label *>(seq[i])->name.compare(label)) == 0) {
			cursor = i;

			break;
		}
}

// Intoarce true daca inputul este in set-ul lui de simboluri,
// false altfel.
bool SingleTransition::FindMatch(char x)
{

	if (!neg_match && to_match->IsMember(x) ||
		neg_match && !to_match->IsMember(x)) {
		if (storing) {
			Env::stack_var.push(x);
		}

		return true;
	}

	return false;
}

// Ruleaza toate masinile din Seq
void SingleTransition::Run()
{
	if (cursor == -1)
		++cursor;

	while (cursor < (int)seq.size()) {
		seq[cursor]->Run();

//		Tape::Print();

		++cursor;

		// Trebuie sa prindem orice Jump event
		if (Env::jump) {
			if (IsThisMyLabel(Env::current_label)) {
				JumpToLabel(Env::current_label);

			} else {
				cursor = (int)seq.size();
			}
		}
	}

	cursor = -1;
	if (storing)
		Env::stack_var.pop();
}

// Self explanatory
void MultiTransition::AddTransition(SingleTransition *tr)
{
	seq.push_back(tr);
}

// Incearca sa faca match pe tranzitii si o ruleaza pe cea
// corecta
void MultiTransition::Run()
{
	if (cursor == -1)
		++cursor;

	while (cursor < (int)seq.size()) {
		if (seq[cursor]->FindMatch(Tape::ElementAtCursor())) {
			seq[cursor]->Run();
			cursor = -1;
			
			return;
		}

		++cursor;
	}

	cursor = -1;
}
