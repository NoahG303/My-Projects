#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"

#include <stack>

class StudentUndo : public Undo {
public:

	virtual ~StudentUndo();
	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:

	struct Change {
		Change(Action action, int row, int col, int amount = 0, std::string str = "") : m_action(action), m_row(row), m_col(col), m_amount(amount), m_str(str) {}
		Action m_action;
		int m_row;
		int m_col;
		int m_amount;
		std::string m_str;
	};
	std::stack<Change> m_cS;

};

#endif // STUDENTUNDO_H_