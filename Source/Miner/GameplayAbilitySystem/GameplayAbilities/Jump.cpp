// Copyright Schuyler Zheng. All Rights Reserved.

#include "GameplayAbilitySystem/GameplayAbilities/Jump.h"
#include "GameplayAbilitySystem/LogAbility.h"
#include "BaseCharacter.h"

UJump::UJump()
{

}

void UJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());    // Cast not castchecked because ACharacter can be destroyed when dead
		UE_LOG(LogAbility, Log, TEXT("Jump Ability activate ability function successfully called for player %s"), *Character->GetDisplayName().ToString());
		Character->Jump();
	}
}

void UJump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ABaseCharacter* Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	UE_LOG(LogAbility, Log, TEXT("Jump Ability end ability function successfully called for player %s"), *Character->GetDisplayName().ToString());
}

void UJump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGameplayAbility_CharacterJump::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->StopJumping();
}
