// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include <typeinfo>
#include "BaseCharacterAttributeSet.h"

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
	// base class handles move, aim and jump inputs
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
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

	// Set up GAS action bindings
	if (IsValid(PlayerInputComponent) && IsValid(AbilitySystemComponent))
	{
		// Bind the ability system component to the input component
		AbilitySystemComponent->BindToInputComponent(PlayerInputComponent);

		// Sprint
		//AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds(TEXT("ConfirmTarget"), TEXT("CancelTarget"),TEXT("EAbilityInputID"), 0, 0));
	}
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
	/**
	* Old code from horror character
	*/

	//// set the sprinting flag
	//bSprinting = true;

	//// are we out of recovery mode?
	//if (!bRecovering)
	//{
	//	// set the sprint walk speed
	//	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	//	// call the sprint state changed delegate
	//	OnSprintStateChanged.Broadcast(true);
	//}

	// Call a virtual input for id 0 (AKA Sprint)	Used instead of getting a abilityspechandle
	AbilitySystemComponent->AbilityLocalInputPressed(0);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Sprint called"));
	UE_LOG(LogTemp, Log, TEXT("Sprint called"));
}

void ABaseCharacter::DoEndSprint()
{
	/**
	* Old code from horror character
	*/

	//// set the sprinting flag
	//bSprinting = false;

	//// are we out of recovery mode?
	//if (!bRecovering)
	//{
	//	// set the default walk speed
	//	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	//	// call the sprint state changed delegate
	//	OnSprintStateChanged.Broadcast(false);
	//}

	// Release input
	AbilitySystemComponent->AbilityLocalInputReleased(0);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Sprint released"));
	UE_LOG(LogTemp, Log, TEXT("Sprint released"));
}

void ABaseCharacter::DoStartCrouch()
{
	Crouch();	// Default crouch

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Crouch started"));
}

void ABaseCharacter::DoEndCrouch()
{
	UnCrouch();	// Default uncrouch

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Crouch ended"));
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
	}
}
