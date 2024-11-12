#include "component.h"
#include "gameObject.h"



// Constructor:
Component::Component()
{
	gameObject = nullptr;
	transform = nullptr;
	isActive = true;
}



// Destructor:
Component::~Component()
{
}



// Public:
bool Component::IsActive()
{
	return isActive && gameObject->isActive;
}
void Component::Update()
{

}