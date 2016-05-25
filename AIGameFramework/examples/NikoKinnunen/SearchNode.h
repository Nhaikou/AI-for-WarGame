#pragma once

#include <iostream>

namespace NikoKinnunen
{
	typedef std::pair<int, int> Position;

	class SearchNode
	{
	public:
		SearchNode* prevNode;		// Tell us the node, where we arrived to this node (previous/parent node)
		Position pos;				// x and y position in the map
		float G;					// Actual distance from the beginning point (A)
		float H;					// Estimeted heuristic distance to end point (B)
		float F;					// G + H
	public:
		SearchNode(const Position &currentPosition, float _H, float deltaG, SearchNode *prev)
			: prevNode(0), pos(currentPosition), G(0), H(_H)
		{
			resetPrev(prev, deltaG);
		}

		// Calculate total G for this node from previous node G value. Also calculates new F from new G and H.
		void resetPrev(SearchNode *prev, float deltaG)
		{
			prevNode = prev;
			if (prev == 0)
			{
				G = 0.0f;
			}
			else
			{
				G = deltaG + prev->G;
			}
			F = H + G;
		}

		// Returns approximation of the total cost (F)
		float distance() const
		{
			return F;
		}

		// Less-than function for sorting
		static bool lessThan(SearchNode *n1, SearchNode *n2)
		{
			return n1->distance() > n2->distance();
		}
	};
}