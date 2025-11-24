// Fill out your copyright notice in the Description page of Project Settings.

#include "Sprint.h"
#include "BaseCharacter.h"
#include "GameplayAbilitySystem/GameplayEffects/UseStamina.h"
#include "BaseCharacterAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitAttributeChange.h"

DEFINE_LOG_CATEGORY(LogSprintAbility);

USprint::USprint()
{
	
}

void USprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Make sure ability is able to start
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sprint ability could not be commited"));
		UE_LOG(LogSprintAbility, Warning, TEXT("Sprint ability could not be activated because it doesn't have authority or prediction key"));
		return;
	}
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sprint ability could not be commited"));
		UE_LOG(LogSprintAbility, Log, TEXT("Sprint ability could not be commited"));
		return;
	}

	// Get the character from the ActorInfo and make sure it is valid
	Character = Cast<ABaseCharacter>(ActorInfo->AvatarActor.Get());
	check(IsValid(Character));
	// Get the attribute set from the AbilitySystemComponent
	AttributeSet = GetAttributeSet(ActorInfo);

	// Call the ApplyStaminaEffect function to apply the UseStamina gameplay effect
	ApplyStaminaEffect(Handle, ActorInfo, ActivationInfo);

	// Set sprint parameters
	Character->GetCharacterMovement()->MaxWalkSpeed = Character->DefaultSprintSpeed;
	Character->GetCharacterMovement()->JumpZVelocity = Character->DefaultSprintJumpHeight;
	
	// Cancel if no more stamina
	WaitStaminaTask = UAbilityTask_WaitAttributeChange::WaitForAttributeChangeWithComparison(this, AttributeSet->GetStaminaAttribute(), FGameplayTag(), FGameplayTag(), EWaitAttributeChangeComparison::LessThanOrEqualTo, 0.0f, false, nullptr);
	WaitStaminaTask->OnChange.AddDynamic(this, &USprint::OnStaminaChanged);
	WaitStaminaTask->ReadyForActivation();
}

void USprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Make sure stuff is valid
	check(IsValid(Character));

	// Reset player max speed to default
	Character->GetCharacterMovement()->MaxWalkSpeed = Character->DefaultWalkSpeed;
	Character->GetCharacterMovement()->JumpZVelocity = Character->DefaultJumpHeight;

	// Remove the UseStamina gameplay effect if it hasn't been removed already
	if (ActiveStaminaEffectHandle.IsValid()) {
		ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveStaminaEffectHandle);
		ActiveStaminaEffectHandle.Invalidate();
	}

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

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void USprint::OnStaminaChanged()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Stamina changed"));
	UE_LOG(LogTemp, Log, TEXT("Stamina changed"));

	check(IsValid(AttributeSet));
	const float NewValue = AttributeSet->GetStamina();

	if (NewValue <= 0.0f) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USprint::ApplyStaminaEffect(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// Lambda function to create a target data handle from an actor. 
	// This could've been normal code, but IDK, Copiolot told me to 
	// (make another function, not a lambda function, but I will never need this. Unless child class, in which I will make this into a real function, but I don't know if I'm going to do that)
	auto MakeTargetDataHandleFromActor = [](AActor* TargetActor)->FGameplayAbilityTargetDataHandle	{
			FGameplayAbilityTargetDataHandle TargetDataHandle;
			check(TargetActor);
			if (TargetActor)
			{
				TArray<TWeakObjectPtr<AActor>> TargetActors;
				TargetActors.Add(TargetActor);

				FGameplayAbilityTargetData_ActorArray* ActorArrayData = new FGameplayAbilityTargetData_ActorArray();
				ActorArrayData->TargetActorArray = TargetActors;

				TargetDataHandle.Add(ActorArrayData);
			}

			return TargetDataHandle;
	};

	// Make sure we have authority to do this
	if (HasAuthority(&ActivationInfo)) {
		// Cuz this isn't server
		return;
	}

	// Get gameplay effect context and stamina effect spec handle
	FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle StaminaEffectSpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(StaminaEffectClass, 1.0f, EffectContext);

	// Set caller magnitude
	check(StaminaEffectSpecHandle.IsValid());
	StaminaEffectSpecHandle.Data->SetSetByCallerMagnitude(StaminaUseGameplayEffectTag, AmountOfStaminaToUse);

	// Log stuff
	UE_LOG(LogTemp, Warning, TEXT("Setting tag: %s value: %f"), *StaminaUseGameplayEffectTag.ToString(), AmountOfStaminaToUse);

	float ValueByTag = StaminaEffectSpecHandle.Data->GetSetByCallerMagnitude(StaminaUseGameplayEffectTag, false);
	float ValueByName = StaminaEffectSpecHandle.Data->GetSetByCallerMagnitude(FName("AttributeChange.UseStamina"), false);

	UE_LOG(LogSprintAbility, Warning, TEXT("Debug: ValueByTag=%f, ValueByName=%f"), ValueByTag, ValueByName);

	// Apply effect to character
	check(StaminaEffectSpecHandle.IsValid());
	ActiveStaminaEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, StaminaEffectSpecHandle);
}
