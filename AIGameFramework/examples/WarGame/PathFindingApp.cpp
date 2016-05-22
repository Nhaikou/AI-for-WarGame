#include "PathFindingApp.h"
#include <Input.h>
#include <ElapsedTimer.h>
#include <StreamTexture.h>
#include <Text.h>
#include "OpenList.h"
#include "ClosedList.h"
#include "SearchLevel.h"
#include "SearchNode.h"


PathFindingApp::PathFindingApp()
{
	m_searchCompleted = false;
	m_searchTimer = 0.0f;

}



PathFindingApp::~PathFindingApp()
{
}


std::vector<slm::vec2> PathFindingApp::doPathfinding(int startX, int startY, int endX, int endY)
{
	bool done = true;
	// Some variables for path finding
	OpenList openList;
	ClosedList closedList;
	SearchLevel searchLevel(mapLayer);
	SearchNode *result = 0;
	std::vector<slm::vec2> mapPoints;

	// Start A* search:
	// Add start node to open list.
	Position startPosition = Position(startX, startY);
	Position endPosition = Position(endX, endY);
	SearchNode *newNode = new SearchNode(startPosition, searchLevel.getH(startPosition, endPosition), 0, 0);
	openList.insertToOpenList(newNode);

	// 1. Get the square on the open list which has the lowest score. Let's call this square S (or prey)
	while (!openList.isEmpty())
	{
		SearchNode *prev = openList.removeSmallestFFromOpenList();
		if (prev->pos == endPosition)
		{
			// Goal found!
			result = prev;
			break;
		}
		else
		{
			// 2. Remove S from the open list and add S to the closed list.
			closedList.addToClosedList(prev);

			// 3. for each square T in S's walkable adjacent tiles:
			std::vector<Position> adjacentNodes = searchLevel.getAdjacentNodes(prev->pos.first, prev->pos.second);
			for (size_t i = 0; i < adjacentNodes.size(); ++i)
			{
				// If T is in the closed list : Ignore it.
				if (closedList.isInClosedList(adjacentNodes[i]))
				{
					continue;
				}

				SearchNode *n = openList.findFromOpenList(adjacentNodes[i]);
				if (n == 0)
				{
					// If T is not in the open list : Add it and compute its score
					SearchNode * newNode = new SearchNode(adjacentNodes[i],
						searchLevel.getH(adjacentNodes[i], endPosition),
						searchLevel.getG(prev, adjacentNodes[i]), prev);
					openList.insertToOpenList(newNode);
				}
				else
				{
					// If T is already in the open list : Check if the F score is lower
					// when we use the current generated path to get there. If it is update
					// it's score and update its parent as well.
					SearchNode *newNode = new SearchNode(adjacentNodes[i],
						searchLevel.getH(adjacentNodes[i], endPosition),
						searchLevel.getG(prev, adjacentNodes[i]), prev);
					if (n->distance() < newNode->distance())
					{
						n->resetPrev(newNode->prevNode, searchLevel.getG(newNode->prevNode, n->pos));
					}
				}
			}
		}
	}
	if (result == 0)
	{
		std::cout << "Path not found!!!\n";
		return mapPoints;
	}

	while (result != 0)
	{
		std::cout << "Path found!\n";
		result = result->prevNode;

		if (result != nullptr)
		{
			slm::vec2 mapPoint;

			mapPoint.x = result->pos.first;
			mapPoint.y = result->pos.second;
			mapPoints.push_back(mapPoint);
		}
	}

	return mapPoints;
}

