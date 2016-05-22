#pragma once

#include "GameEvents.h"
#include "PlayerController.h"
#include "JoystickController.h"
#include "PathFindingApp.h"

using namespace yam2d;

class MadHatterAI : public CharacterController
{
public:
	MadHatterAI(GameObject* owner, GameController* gameController, BotType botType)
		: CharacterController(owner, gameController, botType)
		, m_gameObjectToGo(0)
		, m_reachTolerance(0.0f)
		, m_distanceToDestination(0.0f)
		, m_collisionToHomeBase(false)
		, m_gameObjectToShoot(0)
		, m_predictionDistance(0.0f)
		, m_aimTolerance(0)
		, m_aiObject(owner)
	{
	}

	virtual ~MadHatterAI(void)
	{
	}

	virtual void onMessage(const std::string& msgName, Object* eventObject)
	{
		// Call base class onMessage
		CharacterController::onMessage(msgName, eventObject);
		if (msgName == "Collision")
		{
			CollisionEvent* collisionEvent = dynamic_cast<CollisionEvent*>(eventObject);
			assert(collisionEvent != 0);
			assert(collisionEvent->getMyGameObject() == getGameObject());
			GameObject* otherGo = collisionEvent->getOtherGameObject();
			std::string otherType = otherGo->getType();
			if (otherType == "HomeBase")
			{
				if (hasItem())
				{
					dropItem1();
				}
			}
		}
	}

	// This virtual method is automatically called byt map/layer, when update is called from main.cpp
	virtual void update(float deltaTime)
	{
		/// Call update to base class
		CharacterController::update(deltaTime);
		if (m_gameObjectToGo != 0 && !mapPoints.empty())
		{
			// Move to position
			slm::vec2 movePosition(mapPoints.back().x, mapPoints.back().y);
			m_distanceToDestination = moveDirectToPosition(m_gameObjectToGo->getPosition(), m_reachTolerance);

			if (m_gameObjectToShoot != 0)
			{
				float rotation = m_gameObjectToShoot->getRotation();
				slm::vec2 enemyForwardDir;
				enemyForwardDir.x = cosf(rotation);
				enemyForwardDir.y = sinf(rotation);
				autoUsePrimaryWeapon(m_gameObjectToShoot->getPosition() + m_predictionDistance*enemyForwardDir, m_aimTolerance);
			}
		}

		if (m_distanceToDestination <= m_reachTolerance && !mapPoints.empty())
		{
			mapPoints.pop_back();
		}
	
		// If has collided to home base, then drop bomb.
		else if (m_collisionToHomeBase)
		{
			// Obly if I has flag
			if (hasItem())
			{
				dropItem1();
			}

			m_collisionToHomeBase = false;
		}
	}

	// Enemies to shoot
	void setTargetToShoot(GameObject* gameObjectToShoot, float predictionDistance, float aimTolerance)
	{
		m_gameObjectToShoot = gameObjectToShoot;
		m_predictionDistance = predictionDistance;
		m_aimTolerance = aimTolerance;
	}

	// Reset shooting
	void resetTargetToShoot()
	{
		m_gameObjectToShoot = 0;
		m_predictionDistance = 0.0f;
		m_aimTolerance = 0.0f;
	}

	// Go to the target object
	void setMoveTargetObject(const GameObject* gameObjectToGo, float reachTolerance)
	{
		if (gameObjectToGo == 0)
		{
			resetMoveTargetObject();
			return;
		}

		m_gameObjectToGo = gameObjectToGo;
		m_reachTolerance = reachTolerance;
		findPath();
		m_distanceToDestination = slm::length(m_gameObjectToGo->getPosition() - getGameObject()->getPosition());
		preferPickItem();
	}

	void resetMoveTargetObject()
	{
		m_gameObjectToGo = 0;
		m_reachTolerance = 0.0f;
		m_distanceToDestination = 0.0f;
		stop();
		clearMapPoints();
	}

	float getDistanceToDestination() const
	{
		return m_distanceToDestination;
	}

	void findPath()
	{
		mapPoints = pathFinding->doPathfinding(m_aiObject->getPosition().x, m_aiObject->getPosition().y, m_gameObjectToGo->getPosition().x, m_gameObjectToGo->getPosition().y);
	}

	void setPathFindingApp(PathFindingApp *pathFinding)
	{
		this->pathFinding = pathFinding;
	}

	void clearMapPoints()
	{
		mapPoints.clear();
	}

	std::vector<slm::vec2>& getMapPoints()
	{
		return mapPoints;
	}

private:
	// Path finding
	PathFindingApp *pathFinding;
	std::vector<slm::vec2> mapPoints;

	const GameObject *m_gameObjectToGo, *m_gameObjectToShoot, *m_aiObject;
	float m_reachTolerance;
	float m_distanceToDestination;
	float m_predictionDistance;
	float m_aimTolerance;
	bool m_collisionToHomeBase;
};
