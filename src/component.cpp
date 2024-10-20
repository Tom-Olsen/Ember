#include "component.h"
#include "gameObject.h"



// Constructor:
Component::Component()
{
	gameObject = nullptr;
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