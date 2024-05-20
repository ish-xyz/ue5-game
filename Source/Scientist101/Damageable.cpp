// Fill out your copyright notice in the Description page of Project Settings.


#include "Damageable.h"

// Add default functionality here for any IDamageable functions that are not pure virtual.

float IDamageable::Heal(float)
{
	return 0.0f;
}

bool IDamageable::ReceiveDamage(Damage dmg)
{
	return false;
}

float IDamageable::GetHealth()
{
	return 0.0f;
}

float IDamageable::GetMaxHealth()
{
	return 0.0f;
}
