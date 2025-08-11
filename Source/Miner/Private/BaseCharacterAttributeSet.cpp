// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"

UBaseCharacterAttributeSet::UBaseCharacterAttributeSet()
{

}

void UBaseCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// Clamp values before applying them
	ClampValues(Attribute, NewValue);

	Super::PreAttributeChange(Attribute, NewValue);
}

void UBaseCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// Empty for now...
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

void UBaseCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBaseCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseCharacterAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseCharacterAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseCharacterAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseCharacterAttributeSet, BaseAttack, COND_None, REPNOTIFY_Always);
}
