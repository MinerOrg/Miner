// Fill out your copyright notice in the Description page of Project Settings.

#include "Sprint.h"
#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

void USprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Make sure ability is able to start
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == true) {
		// Get the character from the ActorInfo and make sure it is valid
        Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
		check(IsValid(Character));

		// Increase player max speed
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->DefaultSprintSpeed; 
	}
}
