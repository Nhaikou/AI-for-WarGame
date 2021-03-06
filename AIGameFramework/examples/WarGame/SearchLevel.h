#pragma once
#include <vector>
#include "SearchNode.h"
#include "Texture.h"
#include "PathFindingApp.h"
#include "AIMapLayer.h"

using namespace yam2d;	// For yam2d files

namespace
{
	bool isWhite(unsigned char* p)
	{
		float v = p[0];
		v = v / 255.f;

		if (v == 1)
		{
			return true;
		}
		return false;
	}

	bool isBlack(unsigned char* p)
	{
		float v = p[0];
		v = v / 255.f;
		if (v == 0)
		{
			return true;
		}
		return false;
	}

	bool isGrey(unsigned char* p)
	{
		float v = p[0];
		v = v / 255.f;

		if (v >= 0.4f && v <= 0.6f)
		{
			return true;
		}
		return false;
	}
}

class SearchLevel
{
public:
	SearchLevel(AIMapLayer *inputLayer)
		: inputLayer(inputLayer){}

	// Returns actual distance from "fromNode" to "toPos" (these are adjacent nodes always, so the length is 1)
	float getG(SearchNode *fromNode, const Position &toPos)
	{
		// Return euclidean distance between points
		float dX = (float)(toPos.first - fromNode->pos.first);
		float dY = (float)(toPos.second - fromNode->pos.second);
		float result = sqrtf(dX*dX + dY*dY);
		return result;
	}

	float getH(const Position &fromPos, const Position &toPos)
	{
		float dX = (float)(toPos.first - fromPos.first);
		float dY = (float)(toPos.second - fromPos.second);
		return sqrtf(dX*dX + dY*dY);
	}

	// Returns true, if given point is walkable
	bool isWalkable(int posX, int posY)
	{
		if (posX < 0 || posY < 0 || posX >= inputLayer->getWidth() || posY >= inputLayer->getHeight())
		{
			return false;
		}
		unsigned char *pixel = inputLayer->getPixel(posX, posY);
		bool isNotWalkable = isBlack(pixel);
		return !isNotWalkable;
	}

	std::vector<Position> getAdjacentNodes(int posX, int posY)
	{
		std::vector<Position> result;
		// Add four adjacent nodes to results
		if (isWalkable(posX + 1, posY))
		{
			result.push_back(Position(posX + 1, posY));
		}
		if (isWalkable(posX - 1, posY))
		{
			result.push_back(Position(posX - 1, posY));
		}
		if (isWalkable(posX, posY + 1))
		{
			result.push_back(Position(posX, posY + 1));
		}
		if (isWalkable(posX, posY - 1))
		{
			result.push_back(Position(posX, posY - 1));
		}
		return result;
	}

private:
	AIMapLayer *inputLayer;
};