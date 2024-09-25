#pragma once
#ifndef __INCLUDE_GUARD_component_h__
#define __INCLUDE_GUARD_component_h__



class Component
{
public: // Methods
    virtual ~Component() = default;

    // Virtual method to identify the component type for debugging purposes
    virtual void PrintType() const = 0;
};


#endif // __INCLUDE_GUARD_component_h__