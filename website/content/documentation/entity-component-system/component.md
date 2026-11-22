# Component
The `Component` class is the base class for components attached to an [entity](entity.md).
You can define a new component by deriving from `Component` and adding your own data and methods.

The scene owns each component, and each component is attached to one entity.
An entity can have multiple components of the same type, they are differentiated by the `ComponentId`, which is unique for each component.

`Entity::AddComponent<T>(...)` creates a component and returns a copyable `ComponentHandle<T>`.
`Get()` on the [component handle](#componenthandle) gives borrowed access to the component.
See [IDs and handles](index.md#ids-and-handles).
`Entity::RemoveComponent(handle)` queues the specified component for destruction, and its handle stops resolving immediately.

## ComponentHandle
A `ComponentHandle<T>` gives borrowed access to one component instance of type `T`. Copies refer to the same instance, and a default-constructed handle is invalid.
If you are unsure whether the component exists, check `IsValid()` before calling `Get()`.

| Methods {: .nowrap-first-column } | Explanation |
| --- | --- |
| `bool IsValid() const` | Checks whether the handle resolves to a live component. |
| `T* Get() const` | Returns a borrowed pointer to the component. Requires a valid handle. |
| `operator==`, `operator!=` | Compare the runtime identities held by two handles of the same type. |

## Frame Execution Callbacks
You can override the [Frame Execution Callbacks](index.md#frame-execution-callbacks) to define how a component behaves.
Use the `C++` constructor to initialize the component's own data.
`OnCreate()` runs after the component is attached, so setup can use the owning entity and the initial configuration.
`OnDestroy()` runs before the `C++` destructor and gives the component a chance to clean up.

Access to the owning entity and its transform during `OnDestroy()` is always guaranteed.

## Public native API
| Methods {: .nowrap-first-column } | Explanation |
| --- | --- |
| `void SetIsEnabled(bool)`                        | Enables or disables the component. |
| `const std::string& GetName()`                   | Name of the component. Is set automatically to class name. |
| `bool GetIsEnabled()`                            | Is the component enabled? |
| `bool GetIsActive()`                             | Is the component enabled and its owning entity active? See [Active State](index.md#active-state). |
| `ComponentId GetId()`                            | ID of the component instance. |
| `EntityHandle GetEntity()`                       | Handle to the owning entity. |
| `ComponentHandle<Transform>`<br>`GetTransform()` | Handle to the owning entity's `Transform` component. |
| `ComponentHandle<T>`<br>` GetComponent()`        | Used to access other components attached to the same entity. |

All components also contain the [frame execution callbacks](index.md#frame-execution-callbacks) as virtual void methods:

	OnEnable()
	Start()
	EarlyUpdate()
	FixedUpdate()
	Update()
	LateUpdate()
	OnDisable()
	OnDestroy()

which you can override to achieve the desired behaviour.

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