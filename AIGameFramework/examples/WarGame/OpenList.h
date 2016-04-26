#pragma once
#include <algorithm>
#include <vector>
#include "SearchNode.h"

class OpenList
{
public:
	// Return search node from given position, if it exists. Return 0 otherwise.
	SearchNode *findFromOpenList(const Position &pos)
	{
		for (size_t i = 0; i < openList.size(); ++i)
		{
			if (openList[i]->pos == pos)
			{
				return openList[i];
			}
		}
		return 0;
	}

	// Insert new SearchNode to open list and sorts the open list according to F.
	void insertToOpenList(SearchNode *n)
	{
		openList.push_back(n);
		sortOpenList();
	}

	void sortOpenList()
	{
		std::sort(openList.begin(), openList.end(), &SearchNode::lessThan);
	}

	SearchNode *removeSmallestFFromOpenList()
	{
		SearchNode *result = *(openList.end() - 1);		// Take last one
		openList.pop_back();							// Remove last element
		return result;
	}

	bool isEmpty()
	{
		return openList.empty();
	}
	void clear()
	{
		openList.clear();
	}
private:
	typedef std::vector<SearchNode*>OpenListType;
	OpenListType openList;
};