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


bool PathFindingApp::update(slm::vec2 aiStartPos, slm::vec2 aiEndPos)
{
	
	int width = mapLayer->getWidth();
	int height = mapLayer->getHeight();
	

	if (!m_searchCompleted)
	{
		// Find start and end
		int startX, startY, endX, endY;
		startX = startY = endX = endY = -1;
		
		startX = aiStartPos.x;
		startY = aiStartPos.y;
		
		endX = aiEndPos.x;
		endY = aiEndPos.y;	

		// Update path find!! Set m_searchCompleted to true, when path found, so the texture data is updated.
		if (startX >= 0 && startY >= 0 && endX >= 0 && endY >= 0)
		{
			m_searchCompleted = doPathfinding(startX, startY, endX, endY);
		}
		else
		{
			assert(0);
			return false;
		}
	} // if (!m_searchCompleted)

	return true;
}

bool PathFindingApp::doPathfinding(int startX, int startY, int endX, int endY)
{
	bool done = true;
	// Some variables for path finding
	OpenList openList;
	ClosedList closedList;
	SearchLevel searchLevel(mapLayer);
	SearchNode *result = 0;

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
		printf("Path not found!!!\n");
		return true;
	}

	while (result != 0)
	{
		mapPoints.push_back(slm::vec2(result->pos.first, result->pos.second));
		result = result->prevNode;
	}

	return true;
}

