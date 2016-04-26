#pragma once
#include <vector>
#include "SearchNode.h"
#include "Texture.h"
#include "PathFindingApp.h"

using namespace yam2d;	// For yam2d files

namespace
{
	void setPathColor(yam2d::StreamTexture* t, int x, int y)
	{
		t->getPixel(x, y)[0] = 0xff;
		t->getPixel(x, y)[1] = 0x00;
		t->getPixel(x, y)[2] = 0xff;
	}

	bool isRed(unsigned char* p)
	{
		return p[0] > 200;
	}

	bool isGreen(unsigned char* p)
	{
		return p[1] > 200;
	}

	bool isBlue(unsigned char* p)
	{
		return p[2] > 200;
	}
}

class SearchLevel
{
public:
	SearchLevel(Texture *input)
		: inputTexture(input){}

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
		if (posX < 0 || posY < 0 || posX >= inputTexture->getWidth() || posY >= inputTexture->getHeight())
		{
			return false;
		}
		unsigned char *pixel = inputTexture->getPixel(posX, posY);
		bool isNotWalkable = isGreen(pixel);
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
	Texture *inputTexture;
};