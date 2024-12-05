#include "component.h"
#include "gameObject.h"



// Constructor:
Component::Component()
{
	gameObject = nullptr;
	transform = nullptr;
	scene = nullptr;
	isActive = true;
}



// Destructor:
Component::~Component()
{
}



// Public:
bool Component::IsActive()
{
	// Components can exist without a GameObject, so check if gameObject is nullptr:
	return isActive && (gameObject != nullptr ? gameObject->isActive : true);
}
void Component::Update()
{

}