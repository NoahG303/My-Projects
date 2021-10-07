#include "StudentUndo.h"
using namespace std;

Undo* createUndo()
{
	return new StudentUndo;
}

StudentUndo::~StudentUndo() {
	clear();
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	string s;
	s = ch;
	int amount = 1;

	if (!m_cS.empty()) { // check if batching is doable, and if so, do it
		Change top = m_cS.top();
		if (top.m_action == action) {
			if (top.m_action == INSERT && top.m_row == row && top.m_col == col - 1) {
				m_cS.pop();
				s = top.m_str + ch;
				amount = top.m_amount + 1;
			}
			if (top.m_action == DELETE && top.m_row == row && top.m_col == col) { // delete
				m_cS.pop();
				s = top.m_str + ch;
				amount = top.m_amount + 1;
			}
			if (top.m_action == DELETE && top.m_row == row && top.m_col == col + 1) { // backspace
				m_cS.pop();
				s = ch + top.m_str;
				amount = top.m_amount + 1;
			}
		}
	}

	Change c(action, row, col, amount, s);
	m_cS.push(c);
}

StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) {
	if (m_cS.empty()) { // can't undo nothing!
		return Action::ERROR;
	}

	Change c = m_cS.top();
	m_cS.pop();

	// send most recent action to STE to undo it
	Action actToDo;
	switch (c.m_action) {
		case INSERT: actToDo = DELETE;
			for (int i = 0; i < c.m_amount; i++) {
				c.m_col--;
			}
			break;
		case DELETE: actToDo = INSERT; break;
		case SPLIT:  actToDo = JOIN;   break;
		case JOIN:   actToDo = SPLIT;  break;
		default:	 actToDo = ERROR;  break;
	}

	// send important info to STE
	row = c.m_row;
	col = c.m_col;
	count = c.m_amount;
	text = c.m_str;

	return actToDo;
}

void StudentUndo::clear() {
	while (!m_cS.empty()) {
		m_cS.pop();
	}
}