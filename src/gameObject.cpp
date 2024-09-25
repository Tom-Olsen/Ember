#include "gameObject.h"



// Constructors:
GameObject::GameObject()
{
	this->transform = std::make_shared<Transform>(Float3(0.0f), Float3(0.0f), Float3(1.0f));
}



// Destructor:
GameObject::~GameObject()
{

}



// Public:
template <typename T>
void GameObject::AddComponent(std::shared_ptr<T> component)
{
    static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
    components[typeid(T)] = component;  // Store the component in a shared_ptr
}
template <typename T>
void GameObject::AddComponent(T* component)
{
    static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
    components[typeid(T)] = std::shared_ptr<T>(component);
}

template <typename T>
std::shared_ptr<T> GameObject::GetComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "T must be a Component");
	auto it = components.find(typeid(T));
	if (it != components.end())
		return std::dynamic_pointer_cast<T>(it->second);
	return nullptr;
}

void GameObject::PrintComponents() const
{
	for (const auto& [type, comp] : components)
		comp->PrintType();
}



// Explicit instantiations:
template void GameObject::AddComponent(std::shared_ptr<Camera> component);
template void GameObject::AddComponent(Camera* component);
template std::shared_ptr<Camera> GameObject::GetComponent<Camera>();