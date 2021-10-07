#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cctype>
using namespace std;

bool StudentSpellCheck::isInTrie(string word) const
{
	TrieNode* p = m_rootNode;
	int index;
	int i = 0;
	for (; i < word.size(); i++) { // for each char
		char ch = tolower(word[i]);
		// find array slot where next Node should be
		if (isalpha(ch)) { // letter
			index = ch - 96;
		}
		else { // apostrophe
			index = 0;
		}
		if (p->children[index] == nullptr) { // word DNE, as the next char doesn't follow the current one in any word in the trie's dictionary
			return false;
		}
		p = p->children[index];
	}
	if (p->children[27] != nullptr) { // make sure ending here is a valid word
		return true;
	}
	return false;
}

void StudentSpellCheck::deleteSubtree(TrieNode* p)
{
	if (p == nullptr) { // base case
		return;
	}
	for (int i = 0; i < 28; i++) { // recursively delete all children
		deleteSubtree(p->children[i]);
	}
	delete p; // delete self
	p = nullptr;
}

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::StudentSpellCheck() {
	m_rootNode = new TrieNode;
}

StudentSpellCheck::~StudentSpellCheck() {
	deleteSubtree(m_rootNode); // recursively delete trie
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	ifstream infile(dictionaryFile);
	if (!infile) {
		cerr << "Failed to load " << dictionaryFile << endl;
		return false;
	}
	deleteSubtree(m_rootNode); // recursively delete dictionary
	m_rootNode = new TrieNode; // create a new empty one
	string s;
	TrieNode* p;
	int index;
	while (getline(infile, s)) {
		p = m_rootNode; // starting from the top of the trie
		for (int i = 0; i < s.size(); i++) { // for each char
			char ch = tolower(s[i]);
			if (isalpha(ch)) { // letter
				index = ch - 96;
			}
			else if (ch == 39) { // apostrophe
				index = 0;
			}
			else { // invalid char, ignore it and move to next
				continue;
			}
			if (p->children[index] == nullptr) { // this char ordering DNE yet, add it
				p->children[index] = new TrieNode;
			}
			p = p->children[index]; // move to next letter
		}
		if (p->children[27] != nullptr) { // protect against duplicate words (this word has already been added)
			continue;
		}
		p->children[27] = new TrieNode; // signify end of a word, so it will be recognized as a full word in the trie
	}
	return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	for (int i = 0; i < word.size(); i++) { // make all letters lowercase
		word[i] = tolower(word[i]);
	}
	if (isInTrie(word)) {
		return true;
	}
	suggestions.clear();
	int suggestionsAdded = 0;
	string wordCopy = word;
	char ch;
	for (int i = 0; i < word.size(); i++) { // for each char in word
		if (suggestionsAdded >= max_suggestions) {
			break;
		}
		for (int j = 0; j < 27; j++) { // test the word w/ all possible 1-letter-off options
			if (j == 0) {
				ch = '\'';
			}
			else {
				ch = (96 + j);
			}
			wordCopy[i] = ch;
			if (isInTrie(wordCopy)) { // if this particular option is a word, add it as a suggestion
				suggestions.push_back(wordCopy);
				suggestionsAdded++;
			}
			wordCopy[i] = word[i];
		}
	}
	return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	problems.clear();
	char curr, next;
	int start, end;
	Position p;
	for (int i = 0; i < line.size(); i++) { // iterate thru line chars
		curr = line[i];
		if (isalpha(curr) || curr == 39) { // if current char is apostrophe or letter, it can be in a word. start the word check here
			string s;
			start = i;
			s.push_back(tolower(curr));
			i++;
			// add the full word to string s, tracking its position, until encountering a break character or the end of the line
			while (i < line.size()) {
				next = line[i];
				if (isalpha(next) || next == 39) {
					s.push_back(tolower(next));
					i++;
				}
				else {
					break;
				}
			}
			end = i - 1;
			if (!isInTrie(s)) { // if the word isn't in the dictionary, add its start and end points to the problems vector, to be dealt with by the skeleton
				p.start = start;
				p.end = end;
				problems.push_back(p);
			}
		}
	}
}