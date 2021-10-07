#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:

	StudentSpellCheck();
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:

	struct TrieNode {
		TrieNode() {
			for (int i = 0; i < 28; i++) {
				children[i] = nullptr;
			}
		}
		TrieNode* children[28]; // ' in 0, a-z in 1-26, end of word signifier in 27
	};
	TrieNode* m_rootNode;
	bool isInTrie(std::string word) const;
	void deleteSubtree(TrieNode* p);

};

#endif  // STUDENTSPELLCHECK_H_