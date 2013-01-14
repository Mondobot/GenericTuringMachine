#include "generic.h"

// Adauga o masina la lista lui de masini de rulat
void GenericTM::AddMachine(MT *x)
{
	seq.push_back(x);
}

// Adaugam label-urile la care masina ar putea sari
// (Doar cele NoJump)
void GenericTM::AddLabel(std::string label)
{
	if (!IsThisMyLabel(label))
		labels.push_back(label);
}

// Again, this comment is self explanatory!
bool GenericTM::IsThisMyLabel(std::string label)
{
	bool found = false;

	for (int i = 0; i < (int)labels.size(); ++i)
		if (labels[i].compare(label) == 0) {
			found = true;
			break;
		}

	return found;
}

// Daca pot sa sar la ea, sar!
void GenericTM::JumpToLabel(std::string label)
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

// Ruleaza secventa lui de masini (seq)
void GenericTM::Run()
{
	if (cursor == -1)
		++cursor;

	while (cursor < (int)seq.size()) {
		seq[cursor]->Run();

		++cursor;

		// Prindem evenimentele de jump
		if (Env::jump) {
			if (IsThisMyLabel(Env::current_label)) {
				JumpToLabel(Env::current_label);

			} else {
				cursor = (int)seq.size();
			}
		}

	}

	cursor = -1;
}
