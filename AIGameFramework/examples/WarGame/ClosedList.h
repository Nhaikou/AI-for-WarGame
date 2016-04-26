#pragma once
#include "SearchNode.h"
#include <map>

class ClosedList
{
public:
	ClosedList(){}

	void addToClosedList(SearchNode *node)
	{
		closedList[node->pos] = node;
	}

	bool isInClosedList(const Position &pos)
	{
		return closedList.find(pos) != closedList.end();
	}

	void clear()
	{
		closedList.clear();
	}

private:
	typedef std::map<Position, SearchNode*> ClosedListType;
	ClosedListType closedList;

};