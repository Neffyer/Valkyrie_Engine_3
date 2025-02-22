#pragma once
#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Globals.h"
#include "glmath.h"
#include <vector>
#include "Application.h"
#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentTexture.h"
#include "ComponentCamera.h"
#include "ModuleScene.h"
#include "../External/MathGeoLib/include/Math/float3.h"
#include "ResourceManager.h"


class Component;

class GameObject
{
public:
	GameObject();
	GameObject(std::string name);
	~GameObject();


	void Enable();
	void Disable();

	void Update();
	
	bool SetParent(GameObject* parent);
	void RemoveParent();
	bool IsChildOf(GameObject* parent);
	void DeleteChild(GameObject* child);

	void DebugDraw();

	Component* AddComponent(ComponentTypes component);
	void AddComponent(Component* component);
	Component* GetComponent(ComponentTypes type);
	GameObject* AddChildren(GameObject* children);
	GameObject* GetParent();
	

public:
	std::string mName;
	std::string UID;

	GameObject* parent;
	std::vector<GameObject*> children;

	bool selected = false;
	bool active = true;
	bool deleteGameObject;

	std::vector<Component*> components;

	void UpdateCamera(Camera3D* camera);
	static GameObject* GetGameObjectFromUID(const std::vector<GameObject*>& gameObjects, const std::string& UID);

	//No use, only reference
	ComponentTransform* transform = nullptr;

public:

	ComponentTexture* texture = nullptr;
	ComponentMesh* mesh = nullptr;

public: 

	AABB aabb;
	OBB obb;

};
#endif