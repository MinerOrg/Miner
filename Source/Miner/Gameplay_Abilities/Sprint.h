// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Sprint.generated.h"

class ABaseCharacter;

/**
 * 
 */
UCLASS()
class USprint : public UGameplayAbility
{
	GENERATED_BODY()

protected:	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	ABaseCharacter* Character;
};
