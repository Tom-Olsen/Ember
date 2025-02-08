#include "component.h"
#include "gameObject.h"
#include "scene.h"
#include "transform.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Component::Component()
	{
		m_pGameObject = nullptr;
		m_pTransform = nullptr;
		m_pScene = nullptr;
		isActive = true;
	}
	Component::~Component()
	{

	}



	// Public methods:
	// Getters:
	GameObject* const Component::GetGameObject() const
	{
		return m_pGameObject;
	}
	Transform* const Component::GetTransform() const
	{
		return m_pTransform;
	}
	Scene* const Component::GetScene() const
	{
		return m_pScene;
	}

	// Setters:
	void Component::SetGameObject(GameObject* pGameObject)
	{
		m_pGameObject = pGameObject;
	}
	void Component::SetTransform(Transform* pTransform)
	{
		m_pTransform = pTransform;
	}
	void Component::SetScene(Scene* pScene)
	{
		m_pScene = pScene;
	}

	// Other functionality:
	bool Component::IsActive()
	{
		// Components can exist without a GameObject, so check if gameObject is nullptr:
		return isActive && (m_pGameObject == nullptr ? true : m_pGameObject->isActive);
	}
	void Component::Start()
	{

	}
	void Component::Update()
	{

	}
	void Component::LateUpdate()
	{

	}
}