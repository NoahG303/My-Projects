#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

list<string>::const_iterator StudentTextEditor::const_traverse(int pos) const
{
	list<string>::const_iterator it;
	if (m_lines.size() / 2 > pos) { // pos is in the first half, so start from the beginning
		it = m_lines.begin();
		for (int i = 0; i < pos; i++) {
			it++;
		}
	}
	else { // pos is in the second  half, so start from the end
		it = m_lines.end();
		it--;
		for (int i = m_lines.size() - 1; i > pos; i--) {
			it--;
		}
	}
	return it;
}

list<string>::iterator StudentTextEditor::traverse(int pos)
{
	list<string>::iterator it;
	if (m_lines.size() / 2 > pos) { // pos is in the first half, so start from the beginning
		it = m_lines.begin();
		for (int i = 0; i < pos; i++) {
			it++;
		}
	}
	else { // pos is in the second  half, so start from the end
		it = m_lines.end();
		it--;
		for (int i = m_lines.size() - 1; i > pos; i--) {
			it--;
		}
	}
	return it;
}

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
	m_currRow = 0;
	m_currCol = 0;
	m_redo = false;
	m_lines.push_back(""); // start with an empty string
	m_currNode = m_lines.begin();
}

StudentTextEditor::~StudentTextEditor()
{
	auto it = m_lines.begin(); // doesn't clear the undo stack, as that wouldn't meet time complexity
	while (it != m_lines.end()) {
		it = m_lines.erase(it);
	}
}

bool StudentTextEditor::load(std::string file) {
	ifstream infile(file);
	if (!infile) {
		cerr << "Failed to load " << file << endl;
		return false;
	}
	reset(); // clear current text and reset editor state to blank
	string s;
	while (getline(infile, s)) {
		// process s
		if (s.size() > 0 && s[s.size() - 1] == '\r') { // remove unwanted carriage return char if its there (getline removes \n)
			s.pop_back();
		}
		m_lines.push_back(s);
	}
	m_currNode = m_lines.begin(); // set position to start
	return true;
}

bool StudentTextEditor::save(std::string file) {
	ofstream outfile(file);
	if (!outfile) {
		cerr << "Failed to save " << file << endl;
		return false;
	}
	for (auto it = m_lines.begin(); it != m_lines.end(); it++) { // write lines in editor to file chosen
		outfile << *it << endl;
	}
	return true;
}

void StudentTextEditor::reset() {
	auto it = m_lines.begin();
	while (it != m_lines.end()) {
		it = m_lines.erase(it);
	}
	m_currRow = 0;
	m_currCol = 0;
	getUndo()->clear(); // empty the stack, since the current text is going away
}

void StudentTextEditor::move(Dir dir) {
	switch (dir) {
		case UP:
			if (m_currRow > 0) { // if we aren't on the top, move up
				m_currRow--;
				m_currNode--;
				if (m_currCol > (*m_currNode).size()) { // if our previous line was longer than our current, move to the end of the current (as far right as possible)
					m_currCol = (*m_currNode).size();
				}
			}
			break;
		case DOWN:
			if (m_currRow < m_lines.size() - 1) { // if we aren't on the bottom, move down
				m_currRow++;
				m_currNode++;
				if (m_currCol > (*m_currNode).size()) { // if our previous line was longer than our current, move to the end of the current (as far right as possible)
					m_currCol = (*m_currNode).size();
				}
			}
			break;
		case LEFT:
			if (m_currCol > 0) { // if we aren't on the far left, move left
				m_currCol--;
			}
			else if (m_currRow > 0) { // if we're on the far left of a column that isn't the top row, move to the end of the row above
				m_currRow--;
				m_currNode--;
				m_currCol = (*m_currNode).size();
			}
			break;
		case RIGHT:
			if (m_currCol < (*m_currNode).size()) { // if we aren't on the far right, move right
				m_currCol++;
			}
			else if (m_currRow < m_lines.size() - 1) { // if we're on the far right of a column that isn't the bottom row, move to the start of the row below
				m_currRow++;
				m_currNode++;
				m_currCol = 0;
			}
			break;
		case HOME:
			m_currCol = 0; // move to the far left of the current line
			break;
		case END:
			m_currCol = (*m_currNode).size(); // move to the far right of the current line
			break;
		default:
			break;
	}
}

void StudentTextEditor::del() {
	string s = *m_currNode;
	if (m_currCol < s.size()) { // if we're not on the far right of our row, just decrement line size and shift all chars from ourselves onward one space to the left
		char ch = s[m_currCol];
		for (int i = m_currCol; i < s.size() - 1; i++) {
			s[i] = s[i + 1];
		}
		s.pop_back();
		*m_currNode = s;
		if (!m_redo) { // inform undo to add this action to the stack
			getUndo()->submit(Undo::Action::DELETE, m_currRow, m_currCol, ch);
		}
	}
	else if (m_currRow < m_lines.size() - 1) { // if we are at the end of our row, but we aren't the bottom row, join our line with the one below
		m_currNode++;
		string s2 = *m_currNode;
		m_currNode--;
		*m_currNode = (s + s2);
		auto it = m_currNode;
		it++;
		m_lines.erase(it);
		if (!m_redo) { // inform undo to add this action to the stack
			getUndo()->submit(Undo::Action::JOIN, m_currRow, m_currCol);
		}
	}
}

void StudentTextEditor::backspace() {
	string s = *m_currNode;
	if (m_currCol > 0) { // if we're not on the far left of our row, just decrement line size and shift all chars from 1 before us one space to the left
		char ch = s[m_currCol - 1];
		for (int i = m_currCol - 1; i < s.size() - 1; i++) {
			s[i] = s[i + 1];
		}
		s.pop_back();
		m_currCol--;
		*m_currNode = s;
		if (!m_redo) { // inform undo to add this action to the stack
			getUndo()->submit(Undo::Action::DELETE, m_currRow, m_currCol, ch);
		}
	}
	else if (m_currRow > 0) { // if we are at the beginning of our row, but we aren't the top row, join our line with the one above
		m_currNode--;
		m_currRow--;
		string s2 = *m_currNode;
		m_currCol = s2.size();
		*m_currNode = (s2 + s);
		auto it = m_currNode;
		it++;
		m_lines.erase(it);
		if (!m_redo) { // inform undo to add this action to the stack
			getUndo()->submit(Undo::Action::JOIN, m_currRow, m_currCol);
		}
	}
}

void StudentTextEditor::insert(char ch) {
	if (ch == '\t') { // if ch is a tab, insert 4 spaces
		for (int i = 0; i < 4; i++) {
			insert(' ');
		}
		return;
	}
	string s = *m_currNode;
	s.push_back(ch); // increment string size by 1
	int i = s.size() - 1;
	for (; i > m_currCol; i--) { // shift over all chars
		s[i] = s[i - 1];
	}
	s[i] = ch; // set char to current position
	m_currCol++;
	*m_currNode = s;
	if (!m_redo) { // inform undo to add this action to the stack
		getUndo()->submit(Undo::Action::INSERT, m_currRow, m_currCol, ch);
	}
}

void StudentTextEditor::enter() {
	string s = *m_currNode;
	string s2 = "";
	string s3 = "";
	int i = 0;
	for (; i < m_currCol; i++) { // keep chars up to the cursor on current line
		s2.push_back(s[i]);
	}
	*m_currNode = s2;
	if (!m_redo) { // inform undo to add this action to the stack
		getUndo()->submit(Undo::Action::SPLIT, m_currRow, m_currCol);
	}
	m_currNode++;
	m_currRow++;
	m_currCol = 0;
	for (; i < s.size(); i++) { // copy over all remaining chars to a new line below
		s3.push_back(s[i]);
	}
	m_currNode = m_lines.insert(m_currNode, s3);
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_currRow;
	col = m_currCol;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if (startRow >= m_lines.size() || startRow < 0) { // make sure valid number is passed in
		return 0;
	}
	lines.clear();
	auto it = const_traverse(startRow); // move to the correct initial row
	int count = 0;
	for (int i = 0; i < numRows && it != m_lines.end(); i++, it++) { // add values from m_lines list to lines vector
		lines.push_back(*it);
		count++;
	}
	return count;
}

void StudentTextEditor::undo() {
	int rowToChange, colToChange, amount;
	string textToChange;
	Undo::Action actToDo;
	actToDo = getUndo()->get(rowToChange, colToChange, amount, textToChange); // ask undo for the most recent action
	if (actToDo == Undo::Action::ERROR) {
		return;
	}
	m_currRow = rowToChange; // move to the correct position for the undo
	m_currCol = colToChange;
	m_currNode = traverse(m_currRow);
	m_redo = true; // inform STE that we are redoing an action, so its member functions don't add the undo to the stack (NO REDOING)
	switch (actToDo) {
		case Undo::Action::INSERT: // rTC & cTC are start of where text should be reinserted
			for (int i = 0; i < amount; i++) { // add chars (potentially multiple if batched)
				insert(textToChange[i]);
			}
			m_currCol = colToChange; // return to the proper position, at the start of where the chars were inserted
			break;
		case Undo::Action::DELETE: // rTC & cTC are start of where text should be removed
			for (int i = 0; i < amount; i++) { // delete chars (potentially multiple if batched)
				del();
			}
			break;
		case Undo::Action::SPLIT: // rTC & cTC are at the end of the initial upper line, so calling enter() will undo the line join
			enter(); // re-break lines
			m_currRow--; // return to proper pos, which get() returned (undoes position changing from enter())
			m_currNode--;
			m_currCol = (*m_currNode).size();
			break;
		case Undo::Action::JOIN: // rTC & cTC are where enter key was hit
			del(); // re-join lines
			break;
		default: // ERROR - stack empty - do nothing
			break;
	}
	m_redo = false;
}