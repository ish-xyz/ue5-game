// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SCIENTIST101_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	enum damageType {
		direct = -1, // fallback reaction
		bleeeding = 0, 
		poisoned = 1,
	};

	enum effect {
		none = -1, // fallback reaction
		hit = 0,
		stun = 1,
		knock = 2,
		bleed = 3,
		poison = 4,
	};

	struct Damage
	{
		float value;
		damageType damageType;
		effect effect;
		float effectDuration;
		bool blockable;
		bool interrupt;
	};

	virtual float Heal(float);
	virtual bool  ReceiveDamage(Damage dmg);
	virtual float GetHealth();
	virtual float GetMaxHealth();
};
