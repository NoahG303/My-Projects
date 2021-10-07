#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"

#include <list>

class Undo;

class StudentTextEditor : public TextEditor {
public:

	StudentTextEditor(Undo* undo);
	virtual ~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:

	std::list<std::string> m_lines;
	std::list<std::string>::iterator m_currNode;
	std::list<std::string>::const_iterator const_traverse(int pos) const;
	std::list<std::string>::iterator traverse(int pos);
	int m_currRow;
	int m_currCol;
	bool m_redo;

};

#endif // STUDENTTEXTEDITOR_H_