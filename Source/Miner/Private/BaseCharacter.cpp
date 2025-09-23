// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "BaseCharacterAttributeSet.h"
#include "GameplayAbilitySystem/CharacterAbilities.h"
#include "AbilitySystemComponent.h"
#include <typeinfo>

DEFINE_LOG_CATEGORY(LogBaseCharacter);

ABaseCharacter::ABaseCharacter()
{
	// configure movement (bugged + useless)
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

	// create the ability system component 
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Do not overwrite the member created in the constructor.
	// If for some reason it wasn't created, try to find a component on the actor.
	// Also make sure it is good
	if (!IsValid(AbilitySystemComponent)) {	AbilitySystemComponent = FindComponentByClass<UAbilitySystemComponent>(); }
	checkf(IsValid(AbilitySystemComponent), TEXT("Ability System Component was Invalid on BaseCharacter.cpp"));

	// Get attribute set and validate it.
	AttributeSet = AbilitySystemComponent->GetSet<UBaseCharacterAttributeSet>();
	checkf(AttributeSet != nullptr, TEXT("UBaseCharacterAttributeSet was null on BaseCharacter.cpp"));

	// Grant abilities to the character
	GrantAbilities();

	// Make stamina regen (validate the effect class and the spec before applying)
	checkf(IsValid(StaminaRegenClass), TEXT("Needs a valid staminaregenclass"));

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(StaminaRegenClass, 1.0f, ContextHandle);
	if (!SpecHandle.Data.IsValid()) { AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(), AbilitySystemComponent->ScopedPredictionKey); }
}

void ABaseCharacter::DoJumpStart()
{
	AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(EAbilitiesIndex::JumpAbility));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Jump called"));
	UE_LOG(LogBaseCharacter, Log, TEXT("Jump called"));
}

void ABaseCharacter::DoJumpEnd()
{
	AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(EAbilitiesIndex::JumpAbility));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Jump released"));
	UE_LOG(LogBaseCharacter, Log, TEXT("Jump released"));
}

void ABaseCharacter::DoStartRightClick()
{
	
}

void ABaseCharacter::DoStopRightClick()
{
}

void ABaseCharacter::DoStartLeftClick()
{
}

void ABaseCharacter::DoStopLeftClick()
{
}

void ABaseCharacter::DoStartSprint()
{
	AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(EAbilitiesIndex::SprintAbility));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Sprint called"));
	UE_LOG(LogBaseCharacter, Log, TEXT("Sprint called"));
}

void ABaseCharacter::DoEndSprint()
{
	AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(EAbilitiesIndex::SprintAbility));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Sprint released"));
	UE_LOG(LogBaseCharacter, Log, TEXT("Sprint released"));
}

void ABaseCharacter::DoStartCrouch()
{
	AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(EAbilitiesIndex::CrouchAbility));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Crouch called"));
	UE_LOG(LogBaseCharacter, Log, TEXT("Crouch called"));
}

void ABaseCharacter::DoEndCrouch()
{
	AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(EAbilitiesIndex::CrouchAbility));

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Crouch released"));
	UE_LOG(LogBaseCharacter, Log, TEXT("Crouch released"));
}

void ABaseCharacter::DoSwitchItem()
{
}

//void ABaseCharacter::AttachItemMeshes(ABaseItem* Item)
//{
//}
//
//void ABaseCharacter::PlayLeftClickMontage(UAnimMontage* Montage)
//{
//}
//
//void ABaseCharacter::PlayRightClickMontage(UAnimMontage* Montage)
//{
//}
//
//void ABaseCharacter::AddItemRecoil(float Recoil)
//{
//}
//
//FVector ABaseCharacter::GetItemTargetLocation()
//{
//	return FVector();
//}
//
//void ABaseCharacter::AddItemClass(const TSubclassOf<ABaseItem>& ItemClass)
//{
//}
//
//void ABaseCharacter::OnItemActivated(ABaseItem* Item)
//{
//}
//
//void ABaseCharacter::OnItemDeactivated(ABaseItem* Item)
//{
//}
//
//void ABaseCharacter::OnItemReuse()
//{
//}

void ABaseCharacter::GrantAbilities()
{
	int CurrentAbilityIndex = 0;

	check(IsValid(AbilitySystemComponent));

	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultAbilities)
	{
		checkf(IsValid(AbilityClass), TEXT("Ability Class invalid while trying to grant abilities. Ability type: %s"), ANSI_TO_TCHAR(typeid(AbilityClass).name()));

		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, CurrentAbilityIndex, this));

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Granted ability: %s"), *AbilityClass->GetName()));
		UE_LOG(LogBaseCharacter, Log, TEXT("Granted ability: %s"), *AbilityClass->GetName());

		CurrentAbilityIndex++;
	}
}
