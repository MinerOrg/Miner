// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilitySystem/GameplayAbilities/Crouch.h"
#include "Crouch.h"
#include "BaseCharacter.h"

DEFINE_LOG_CATEGORY(LogCrouchAbility);


UCrouch::UCrouch()
{

}

void UCrouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Crouch ability could not be commited"));
		UE_LOG(LogCrouchAbility, Warning, TEXT("Crouch ability could not be activated because it doesn't have authority or prediction key"));
		return;
	}
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Crouch ability could not be commited"));
		UE_LOG(LogTemp, Warning, TEXT("Crouch ability could not be committed"));
		return;
	}

	Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor);
	Character->Crouch();
}

void UCrouch::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	check(IsValid(Character));
	Character->UnCrouch();
}

void UCrouch::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
