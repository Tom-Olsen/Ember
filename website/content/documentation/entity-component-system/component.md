# Component
The `Component` class is the base class for components attached to an [entity](entity.md).
You can define a new component by deriving from `Component` and adding your own data and methods.

The world owns each component, and each component is attached to one entity.
An entity can have multiple components of the same type, and they are differentiated by the `ComponentId`, which is unique for each component.

`Entity::AddComponent<T>(...)` creates a component and returns a copyable `ComponentHandle<T>`.
`Get()` on the [component handle](#componenthandle) gives borrowed access to the component.
See [IDs and handles](index.md#ids-and-handles).
`Entity::RemoveComponent(handle)` queues the specified component for destruction, and its handle stops resolving immediately.

## ComponentHandle
A `ComponentHandle<T>` gives borrowed access to one component instance of type `T`.
Copies refer to the same instance, and a default-constructed handle is invalid.
If you are unsure whether the component exists, check `IsValid()` before calling `Get()`.

| Methods {: .nowrap-first-column } | Explanation |
| --- | --- |
| `bool IsValid() const` | Checks whether the handle resolves to a live component. |
| `T* Get() const` | Returns a borrowed pointer to the component. Requires a valid handle. |
| `operator==`, `operator!=` | Compare the runtime identities held by two handles of the same type. |

## Copying components
`Component` has a protected copy constructor that copies the local enabled flag.
The copy starts detached with fresh lifecycle state and receives a new `ComponentId` when attached to its new entity.
Copy assignment is deleted.

A derived component can use its default copy constructor, define its own copy constructor for state that needs special handling, or delete it.
`Entity::Copy()` skips components that cannot be copy-constructed.
Copied components go through the usual lifecycle when attached to the new entity.

## Callbacks
You can override the [lifecycle](index.md#lifecycle-callbacks), [update](index.md#update-callbacks), and [deferred](index.md#deferred-callbacks) callbacks to define how a component behaves.
Use the `C++` constructor to initialize the component's own data.
`OnCreate()` runs after the component is attached. Here you have access to the owning entity and its transform.
`Start()` runs before the first update callback. Here you have access to other components attached to the same entity.
`OnDestroy()` runs before the `C++` destructor and gives the component a chance to clean up.
During `OnDestroy()`, `GetEntity()` and `GetTransform()` provide access to the owning entity and its transform.
Handles obtained through these getters are valid until the `OnDestroy()` callback returns.
Previously expired handles stay expired.

## Public native API
| Methods {: .nowrap-first-column } | Explanation |
| --- | --- |
| `void SetIsEnabled(bool)`                        | Enables or disables the component. |
| `const std::string& GetName()`                   | Name of the component. It is set automatically to the class name. |
| `bool GetIsEnabled()`                            | Is the component enabled? |
| `bool GetIsActive()`                             | Is the component enabled and its owning entity active? See [Active State](index.md#active-state). |
| `ComponentId GetId()`                            | ID of the component instance. |
| `EntityHandle GetEntity()`                       | Handle to the owning entity. Safe to use in `OnCreate()`.|
| `ComponentHandle<Transform>`<br>`GetTransform()` | Handle to the owning entity's `Transform` component. Safe to use in `OnCreate()`.|
| `ComponentHandle<T>`<br>`GetComponent()`         | Used to access other components attached to the same entity. Safe to use in `Start()`, but not in `OnCreate()`. |

All components also contain the [lifecycle](index.md#lifecycle-callbacks), [update](index.md#update-callbacks), and [deferred](index.md#deferred-callbacks) callbacks as virtual void methods:

	OnCreate()
	OnEnable()
	OnDisable()
	Start()
	EarlyUpdate()
	FixedUpdate()
	Update()
	LateUpdate()
	OnDestroy()

You can override these methods to achieve the desired behaviour.

## Examples
Here is a simple component that stores an entity's health.

```cpp
class Health : public Component
{
private: // Members:
    int hitPoints;

public: // Methods:
    explicit Health(int hitPoints)
        : hitPoints(hitPoints)
    {

    }

    void ApplyDamage(int damage)
    {
        hitPoints -= damage;
    }
};
```

The [ECS example](index.md#examples) shows how to attach and use this component.