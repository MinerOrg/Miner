// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAttributeSet.h"

void UBaseCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampValues(Attribute, NewValue);
}

void UBaseCharacterAttributeSet::ClampValues(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Clamp the health value to be between 0 and MaxHealth
	if (Attribute == GetHealthAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
		return;
	}

	// Clamp the stamina value to be between 0 and MaxStamina
	if (Attribute == GetStaminaAttribute()) {
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
}
