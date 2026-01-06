// Copyright Schuyler Zheng. All Rights Reserved.

#include "GameplayAbilitySystem/GameplayAbilities/Crouch.h"
#include "BaseCharacter.h"
#include "GameplayAbilitySystem/LogAbility.h"

UCrouch::UCrouch()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

void UCrouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		ABaseCharacter* Character = CastChecked<ABaseCharacter>(ActorInfo->AvatarActor.Get());
		UE_LOG(LogAbility, Log, TEXT("Crouch Ability activate ability function successfully called for player %s"), *Character->GetDisplayName().ToString());
		Character->Jump();
	}
}

void UCrouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor);
	UE_LOG(LogAbility, Log, TEXT("Crouch Ability end ability function successfully called for player %s"), *Character->GetDisplayName().ToString());	
	Character->UnCrouch();
}

void UCrouch::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
