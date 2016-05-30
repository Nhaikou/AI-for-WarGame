#include "NikoKinnunen.h"

#include "GameApp.h"
#include "GameEvents.h"
#include "PlayerController.h"
#include "JoystickController.h"
#include "PathFindingApp.h"
#include "Layer.h"

namespace NikoKinnunen
{
	using namespace yam2d;

	class NikoKinnunenController : public CharacterController
	{
	public:
		NikoKinnunenController(GameObject* owner, GameController* gameController, BotType botType)
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

		virtual ~NikoKinnunenController(void)
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
				m_distanceToDestination = moveDirectToPosition(movePosition, m_reachTolerance);

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

	class NikoKinnunenPlayer : public PlayerController
	{
	private:
		std::string m_myTeamName;
		// NikoKinnunenController
		std::vector< yam2d::Ref<NikoKinnunenController>> m_NikoKinnunenControllerControllers;
		std::vector< yam2d::Ref<JoystickController> > m_joystickControllers;

		PathFindingApp *pathFindingApp;
		AIMapLayer *mapLayer;

	public:
		NikoKinnunenPlayer()
			: PlayerController()
		{
			pathFindingApp = new PathFindingApp();
		}

		virtual ~NikoKinnunenPlayer()
		{
			delete pathFindingApp;
		}


		virtual void setMyTeamName(std::string& teamName)
		{
			m_myTeamName = teamName;
		}

		// Called, when bots are spawn. Shall return correct player character controller for bot if given name.
		virtual CharacterController* createPlayerCharacterController(GameController* gameController, yam2d::GameObject* ownerGameObject, const std::string& playerName, BotType type)
		{
			// NikoKinnunenController
			if (playerName == "NikoKinnunen")
			{
				NikoKinnunenController* m_NikoKinnunenController = new NikoKinnunenController(ownerGameObject, gameController, type);
				m_NikoKinnunenController->setPathFindingApp(pathFindingApp);
				m_NikoKinnunenControllerControllers.push_back(m_NikoKinnunenController);
				return m_NikoKinnunenController;
			}
			return 0;
		}

		// Called, when bots have spawn. Can be used some custom initialization after spawn.
		virtual void onGameStarted(GameEnvironmentInfoProvider* environmentInfo)
		{
			yam2d::esLogMessage("onGameStarted");
			// Start going straight to dynamite
			const yam2d::GameObject* dynamite = environmentInfo->getDynamite();

			// Map colors
			uint8_t RED_PIXEL[4] = { 0xff, 0x00, 0x00, 0xff };
			uint8_t GREEN_PIXEL[4] = { 0x00, 0xff, 0x00, 0xff };
			uint8_t BLUE_PIXEL[4] = { 0x00, 0x00, 0xff, 0xff };
			uint8_t TP_PIXEL[4] = { 0x00, 0x00, 0x00, 0x00 };

			// NikoKinnunenController
			std::vector<slm::vec2> mapLayerPoints;
			mapLayer = environmentInfo->getAILayer("GroundMoveSpeed"); // Pass this instead of image
			pathFindingApp->setMapLayer(mapLayer);
			const yam2d::GameObject* homeBase = environmentInfo->getMyHomeBase(this);
			homeBase->getName();
			
			for (size_t i = 0; i < m_NikoKinnunenControllerControllers.size(); ++i)
			{
				if (m_NikoKinnunenControllerControllers[i]->isRobot())
				{
					m_NikoKinnunenControllerControllers[i]->setMoveTargetObject(homeBase, 3.0f);

					/*if (m_NikoKinnunenControllerControllers[i]->getGameObject()->getName() == "BotSpawn0")
					{
						m_NikoKinnunenControllerControllers[i]->resetMoveTargetObject();
						m_NikoKinnunenControllerControllers[i]->setMoveTargetObject(dynamite, 1.0f);
					}*/
					
				}
				else if (m_NikoKinnunenControllerControllers[i]->isSoldier())
				{
					m_NikoKinnunenControllerControllers[i]->setMoveTargetObject(dynamite, 1.0f);
				}
			}
		}


		// Called when game has ended. Can be used some cuystom deinitialization after game.
		virtual void onGameOver(GameEnvironmentInfoProvider* environmentInfo, const std::string& gameResultString)
		{
			yam2d::esLogMessage("onGameOver: %s wins!", gameResultString.c_str());

			// NikoKinnunenController reset shooting
			for (size_t i = 0; i < m_NikoKinnunenControllerControllers.size(); ++i)
			{
				m_NikoKinnunenControllerControllers[i]->resetTargetToShoot();
				m_NikoKinnunenControllerControllers[i]->resetMoveTargetObject();
			}
		}

		// Called each frame. Update you player character controllers in this function.
		virtual void onUpdate(GameEnvironmentInfoProvider* environmentInfo, float deltaTime)
		{
			//	yam2d::esLogMessage("onUpdate");
		}

		// Called, when game event has ocurred.
		// Event name tells the event in question, each event type may have eventObject data associated with the event.
		// 
		// Currently supported eventNames and eventObject data desctiption:
		// - ObjectSpawned:
		//		Called when new object is spawned to the level.
		//		eventObject: yam2d::GameObject* = object which was spawn.
		// - ObjectDeleted:
		//		Called when object is deleted from the level.
		//		eventObject: yam2d::GameObject* = object which is going to be deleted.
		// - ItemPicked: 
		//		Called when item has beed picked by game object.
		//		eventObject: ItemPickedEvent* = Item picked event
		// - ItemDropped: 
		//		Called when item has beed dropped by game object.
		//		eventObject: yam2d::GameObject* = object which was dropped  (Dynamite).
		virtual void onGameEvent(GameEnvironmentInfoProvider* environmentInfo, const std::string& eventName, yam2d::Object* eventObject)
		{
			if (eventName == "ObjectSpawned" || eventName == "ObjectDeleted")
			{
				yam2d::GameObject* gameObject = dynamic_cast<yam2d::GameObject*>(eventObject);
				assert(gameObject != 0);
				std::string type = gameObject->getType();
				// Don't print spawned weapon projectiles or explosions.
				if (type != "Missile"
					&& type != "Bullet"
					&& type != "Grenade"
					&& type != "Mine"
					&& type != "SmallExplosion"
					&& type != "MediumExplosion"
					&& type != "BigExplosion"
					&& type != "MineExplosion"
					&& type != "GrenadeExplosion")
				{
					// Prints: Soldier, Robot, HomeBase, Flag
					if (gameObject->getProperties().hasProperty("team"))
					{
						std::string teamName = gameObject->getProperties()["team"].get<std::string>();
						yam2d::esLogMessage("%s: gameObjectType=%s, teamName=%s", eventName.c_str(), gameObject->getType().c_str(), teamName.c_str());
					}
					else
					{
						yam2d::esLogMessage("%s: gameObjectType=%s", eventName.c_str(), gameObject->getType().c_str());
					}
				}
			}
			else if (eventName == "ItemPicked")
			{
				ItemEvent* itemEvent = dynamic_cast<ItemEvent*>(eventObject);
				assert(itemEvent != 0);
				yam2d::GameObject* item = itemEvent->getItemGameObject();
				int teamIndex = itemEvent->getCharacterGameObject()->getProperties()["teamIndex"].get<int>();

				yam2d::esLogMessage("%s: gameObjectType=%s, team=%d", eventName.c_str(), itemEvent->getItemGameObject()->getType().c_str(), teamIndex);

				// NikoKinnunenController shoot
				for (size_t i = 0; i < m_NikoKinnunenControllerControllers.size(); ++i)
				{
					m_NikoKinnunenControllerControllers[i]->setTargetToShoot(itemEvent->getCharacterController()->getGameObject(), 1.9f, 0.05f);
				}

				if (teamIndex == getMyTeamIndex())
				{
					// My team picked item. 
					// Go to enemy home base.
					const yam2d::GameObject* enemyBase = environmentInfo->getEnemyHomeBase(this);

					// NikoKinnunenController
					for (size_t i = 0; i < m_NikoKinnunenControllerControllers.size(); ++i)
					{
						m_NikoKinnunenControllerControllers[i]->resetTargetToShoot();
						m_NikoKinnunenControllerControllers[i]->setMoveTargetObject(enemyBase, 1.0f);
					}
				}
				else
				{
					// Other team picked the item.
					// Go to enemy's enemy == me home base.
					const yam2d::GameObject* homeBase = environmentInfo->getMyHomeBase(this);

					// NikoKinnunenController
					for (size_t i = 0; i < m_NikoKinnunenControllerControllers.size(); ++i)
					{
						m_NikoKinnunenControllerControllers[i]->setMoveTargetObject(homeBase, 1.0f);
						m_NikoKinnunenControllerControllers[i]->stop();
					}
				}
			}
			else if (eventName == "ItemDropped")
			{
				ItemEvent* itemEvent = dynamic_cast<ItemEvent*>(eventObject);
				assert(itemEvent != 0);
				yam2d::esLogMessage("%s: gameObjectType=%s", eventName.c_str(), itemEvent->getItemGameObject()->getType().c_str());

				// Item propped.
				// Start going straight to dynamite
				const yam2d::GameObject* dynamite = environmentInfo->getDynamite();

				// NikoKinnunenController
				for (size_t i = 0; i < m_NikoKinnunenControllerControllers.size(); ++i)
				{
					m_NikoKinnunenControllerControllers[i]->resetTargetToShoot();
					m_NikoKinnunenControllerControllers[i]->setMoveTargetObject(dynamite, 1.0f);
					m_NikoKinnunenControllerControllers[i]->preferPickItem();
				}
			}
			else
			{
				assert(0); // Should be impossible, because unknown message id.
			}
		}

		// Send, when game object receives an event.
		// Event name tells the event in question, each event type may have eventObject data associated with the event.
		//
		// Currently supported eventNames and eventObject data desctiption:
		// - Collision: 
		//		Called when game object collides to another object. 
		//		eventObject: CollisionEvent* = Information about collision.
		// - TakingDamage:
		//		Called when game object is taking damage.
		//		eventObject: yam2d::GameObject* = object which give damage to the object. Typically this is bullet, missile, mine or grenade etc.
		// - ZeroHealth: 
		//		Called when game object is going to die, because of zero health.
		//		eventObject: 0 always.
		virtual void onGameObjectEvent(GameEnvironmentInfoProvider* environmentInfo, yam2d::GameObject* gameObject, const std::string& eventName, yam2d::Object* eventObject)
		{
			if (eventName == "Collision")
			{
				CollisionEvent* collisionEvent = dynamic_cast<CollisionEvent*>(eventObject);
				assert(collisionEvent != 0);
				if (!collisionEvent->isValid()) return;
				yam2d::GameObject* otherGo = collisionEvent->getOtherGameObject();
				std::string otherType = otherGo->getType();
				yam2d::vec2 localNormal = collisionEvent->getLocalNormal();
			}
			else if (eventName == "TakingDamage")
			{
				int teamIndex = gameObject->getProperties()["teamIndex"].get<int>();

				TakingDamageEvent* damageEvent = dynamic_cast<TakingDamageEvent*>(eventObject);
				yam2d::GameObject* damageFromObject = damageEvent->getFromObject();
				float newHealth = damageEvent->getNewHealth();
				//	yam2d::esLogMessage("%s(team=%d) %s: fromObject=%s. New health: %3.1f", gameObject->getType().c_str(), teamIndex, eventName.c_str(), damageFromObject->getType().c_str(), newHealth);
			}
			else if (eventName == "ZeroHealth")
			{
				int teamIndex = gameObject->getProperties()["teamIndex"].get<int>();
				yam2d::esLogMessage("%s(team=%d) %s.", gameObject->getType().c_str(), teamIndex, eventName.c_str());
			}
			else
			{
				assert(0); // Should be impossible, because unknown message id.
			}
		}
	};

	PlayerController *createNewPlayer()
	{
		return new NikoKinnunenPlayer();
	}

}