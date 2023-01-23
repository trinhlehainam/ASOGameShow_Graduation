#pragma once
#include "IComponent.h"

/// <param name="maxHealth"></param>
class HealthComponent :
    public IComponent
{
private:
    int health_;
    int maxHealth_;
    float takenDamage_;                 // damage resist (unit : percentage)
    int receivedDamage_;                // Latest Damage Received
    public:

    HealthComponent(const std::shared_ptr<Entity>& owner, const int& maxHealth);
    ~HealthComponent() = default;

    void Init() override;
    void Update(float deltaTime) override;
    void Render() override;

    void SetMaxHealth(const int& maxHealth);
    inline int GetMaxHealth() const { return maxHealth_; }
    void SetHealth(const int& health);
    inline int Health() const { return health_; }
    inline int ReceivedDamage() const { return receivedDamage_; }
    void TakeDamage(const int& damage);
    void AddMaxHealth(const int& health);
    void Heal(const int& health);
};