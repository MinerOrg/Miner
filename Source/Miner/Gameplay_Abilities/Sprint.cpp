// Fill out your copyright notice in the Description page of Project Settings.

#include "Sprint.h"
#include "BaseCharacter.h"
#include "Gameplay_Abilities/UseStamina.h"
#include "BaseCharacterAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChange.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

USprint::USprint()
{
	
}

void USprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Make sure ability is able to start
	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		// Get the character from the ActorInfo and make sure it is valid
		Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
		check(IsValid(Character));
		// Get the attribute set from the AbilitySystemComponent
		const UBaseCharacterAttributeSet* AttributeSet = GetAttributeSet(ActorInfo);
		// Get gameplay effect handle for stamina drain
		ActiveStaminaEffectHandle = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, Cast<UGameplayEffect>(UUseStamina::StaticClass()->GetDefaultObject()), 1, 1);

		// Increase player max speed and print debug message
		Character->GetCharacterMovement()->MaxWalkSpeed = Character->DefaultSprintSpeed;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Sprint started"));
		UE_LOG(LogTemp, Log, TEXT("Sprint started"));

		// Cancel if no more stamina
		WaitStaminaTask = UAbilityTask_WaitAttributeChange::WaitForAttributeChangeWithComparison(this, AttributeSet->GetStaminaAttribute(), FGameplayTag(), FGameplayTag(), EWaitAttributeChangeComparison::LessThanOrEqualTo, 0.0f, false, nullptr);
		WaitStaminaTask->OnChange.AddDynamic(this, &USprint::OnStaminaChanged);
		WaitStaminaTask->ReadyForActivation();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sprint ability could not be commited"));

		return;
	}
}

void USprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Make sure stuff is valid
	check(IsValid(Character));
	//check(ActiveStaminaEffectHandle.IsValid());

	// Reset player max speed to default
	Character->GetCharacterMovement()->MaxWalkSpeed = Character->DefaultWalkSpeed;

	// Remove the UseStamina gameplay effect
	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveStaminaEffectHandle);
	ActiveStaminaEffectHandle.Invalidate();

	// Clean up wait stamina task
	WaitStaminaTask->EndTask();
	WaitStaminaTask = nullptr;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Sprint ended"));
	UE_LOG(LogTemp, Log, TEXT("Sprint ended"));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);	// Do everything before, so the object doesn't get deleted before we have to still do stuff
}

void USprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void USprint::OnStaminaChanged()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Stamina changed"));
	UE_LOG(LogTemp, Log, TEXT("Stamina changed"));

	const UBaseCharacterAttributeSet* AttributeSet = GetAttributeSet(CurrentActorInfo);
	check(IsValid(AttributeSet));
	const float NewValue = AttributeSet->GetStamina();

	if (NewValue <= 0.0f) {
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}
