// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "BaseCharacterAttributeSet.h"

ABaseCharacter::ABaseCharacter()
{
	// configure movement (bugged + useless)
	//GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

	// create the ability system component 
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	checkf(IsValid(AbilitySystemComponent), TEXT("Ability System Component was Invalid on BaseCharacter.cpp"));

	AttributeSet = AbilitySystemComponent->GetSet<UBaseCharacterAttributeSet>();
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent = GetAbilitySystemComponent();

	// Set the Ability System Component
	checkf(IsValid(AbilitySystemComponent), TEXT("Ability System Component was Invalid on BaseCharacter.cpp"));
	AttributeSet = AbilitySystemComponent->GetSet<UBaseCharacterAttributeSet>();
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

}

void ABaseCharacter::DoEndSprint()
{
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
}

void ABaseCharacter::DoStartCrouch()
{
	Crouch();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Crouch started"));
}

void ABaseCharacter::DoEndCrouch()
{
	UnCrouch();
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
