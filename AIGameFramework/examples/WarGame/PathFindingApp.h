#ifndef PATHFINDING_APP_H_
#define PATHFINDING_APP_H_
#include <Object.h>
#include <es_util.h>
#include <Sprite.h>
#include <AIMapLayer.h>
#include <GameObject.h>
#include "SpriteSheet.h"


class PathFindingApp : public yam2d::Object
{
	// Public typedefs and methods:
public:
	PathFindingApp();
	virtual ~PathFindingApp();

	// Updates the app
	//bool update(slm::vec2 aiStartPos, slm::vec2 aiEndPos);

	std::vector<slm::vec2> doPathfinding(int startX, int startY, int endX, int endY);
	
	void setMapLayer(AIMapLayer *mapLayer){ this->mapLayer = mapLayer; }

	// std::vector<slm::vec2> getMapPoints(){ return mapPoints; }

	// Private member variables and methods:
private:
	AIMapLayer *mapLayer;
	std::vector<slm::vec2> mapPoints;

	bool m_searchCompleted = false;
	float m_searchTimer;

	// Hidden copy constructor and assignment operator.
	PathFindingApp(const PathFindingApp&);
	PathFindingApp& operator=(const PathFindingApp&);
};

#endif

