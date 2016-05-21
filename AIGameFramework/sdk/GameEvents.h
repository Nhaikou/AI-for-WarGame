// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// War Game for GameAIGameFramework.
//
// Copyright (c) 2016 Mikko Romppainen. All Rights reserved.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef GAME_EVENTS_H_
#define GAME_EVENTS_H_ 

// Include base class
#include <Object.h>
#include <GameObject.h>


class PhysicsBody;
class b2Body;
class CharacterController;
class PickableItemController;

class CollisionEvent : public yam2d::Object
{
public:
	CollisionEvent(yam2d::GameObject* goA, yam2d::GameObject* goB);
//CollisionEvent(b2Body* bA, b2Body* bB, const yam2d::vec2& localNormal = yam2d::vec2(0.0f));
	CollisionEvent(PhysicsBody* goA, PhysicsBody* goB, const yam2d::vec2& localNormal = yam2d::vec2(0.0f));

	virtual ~CollisionEvent();

	bool isValid() const;

	//PhysicsBody* getMyBody() const { return m_myBody; }
	//PhysicsBody* getOtherBody() const { return m_otherBody; }
	yam2d::GameObject* getMyGameObject() const;
	yam2d::GameObject* getOtherGameObject() const;

	const yam2d::vec2& getLocalNormal() const { return m_localNormal; }
	//b2Body* getBodyA() const { return m_bA; }
	//b2Body* getBodyB() const { return m_bB; }
	bool isCollisionToSelf() const;
private:
	PhysicsBody* m_myBody;
	PhysicsBody* m_otherBody;
	yam2d::vec2 m_localNormal;
	yam2d::GameObject* m_goA;
	yam2d::GameObject* m_goB;
};


class TakingDamageEvent : public yam2d::Object
{
public:
	TakingDamageEvent(yam2d::GameObject* fromObject, float damageAmount, float newHealth);

	virtual ~TakingDamageEvent();

	yam2d::GameObject* getFromObject() const { return m_fromObject; }
	float getDamageAmount() const { return m_damageAmount; }
	float getNewHealth() const { return m_newHealth; }
private:
	yam2d::GameObject* m_fromObject;
	float m_damageAmount;
	float m_newHealth;
};

class ItemEvent : public yam2d::Object
{
public:
	ItemEvent(CharacterController* object, PickableItemController* item);

	virtual ~ItemEvent();

	yam2d::GameObject* getCharacterGameObject() const;
	yam2d::GameObject* getItemGameObject() const;

	// Which character controller picked the item.
	CharacterController* getCharacterController() const { return m_object; }
	PickableItemController* getItem() const { return m_item; }
private:
	CharacterController* m_object;
	PickableItemController* m_item;
};


#endif