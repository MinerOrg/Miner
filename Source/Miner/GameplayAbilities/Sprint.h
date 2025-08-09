// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Logging/LogMacros.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "BaseCharacterAttributeSet.h"
#include "Sprint.generated.h"

class ABaseCharacter;
class UAbilityTask_WaitAttributeChange;
class UAbilityTask_WaitInputRelease;

DECLARE_LOG_CATEGORY_EXTERN(LogSprintAbility, Log, All);

/**
 * Sprint ability class
 */
UCLASS()
class USprint : public UGameplayAbility
{
	GENERATED_BODY()

	USprint();

protected:	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	ABaseCharacter* Character;

	UPROPERTY(EditDefaultsOnly)
	float AmountOfStaminaToUse = 10.0f;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag StaminaUseGameplayEffectTag;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StaminaEffectClass;

	/** The handle for the stamina effect, created when the effect is first called */
	FActiveGameplayEffectHandle ActiveStaminaEffectHandle;	// Handle for the UseStamina gameplay effect
	/** AbilityTask to monitor stamina changes */
	UPROPERTY()
	UAbilityTask_WaitAttributeChange* WaitStaminaTask;
	/* Function that waits for release */
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	/** Stamina changed call this function */
	UFUNCTION()
	void OnStaminaChanged();

	/** Helper function to get the attribute set */
	const UBaseCharacterAttributeSet* GetAttributeSet(const FGameplayAbilityActorInfo* ActorInfo) { return Cast<const UBaseCharacterAttributeSet>(ActorInfo->AbilitySystemComponent->GetAttributeSet(UBaseCharacterAttributeSet::StaticClass())); }
	void ApplyStaminaEffect(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);
};
