// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "BaseCharacterAttributeSet.h"
#include "GameplayAbilitySystem/CharacterAbilities.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
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

	AbilitySystemComponent = GetAbilitySystemComponent();

	// Set the Ability System Component
	checkf(IsValid(AbilitySystemComponent), TEXT("Ability System Component was Invalid on BaseCharacter.cpp"));
	AttributeSet = AbilitySystemComponent->GetSet<UBaseCharacterAttributeSet>();

	// Grant abilities to the character
	GrantAbilities();
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABaseCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABaseCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::LookInput);

		// Left click
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &ABaseCharacter::DoStartLeftClick);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &ABaseCharacter::DoStopLeftClick);

		// Right click
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Started, this, &ABaseCharacter::DoStartRightClick);
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Completed, this, &ABaseCharacter::DoStopRightClick);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABaseCharacter::DoStartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABaseCharacter::DoEndSprint);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABaseCharacter::DoStartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ABaseCharacter::DoEndCrouch);

		// Switch item
		EnhancedInputComponent->BindAction(SwitchItemAction, ETriggerEvent::Triggered, this, &ABaseCharacter::DoSwitchItem);
	}
	else {
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	// Set up GAS action bindings
	if (IsValid(PlayerInputComponent) && IsValid(AbilitySystemComponent))
	{
		// Bind the ability system component to the input component
		AbilitySystemComponent->BindToInputComponent(PlayerInputComponent);

		// Sprint
		//AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds(TEXT("ConfirmTarget"), TEXT("CancelTarget"),TEXT("EAbilityInputID"), 0, 0));
	}
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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Sprint released"));
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
