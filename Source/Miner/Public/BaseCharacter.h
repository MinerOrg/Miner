// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinerCharacter.h"
#include "AbilitySystemInterface.h"
#include "Logging/LogMacros.h"
#include "BaseCharacter.generated.h"

class ABaseItem;
class AShooterWeapon;

DECLARE_LOG_CATEGORY_EXTERN(LogBaseCharacter, Log, All);

/**
 * Base Character class used for player character, enemies, etc.
 */
UCLASS()
class MINER_API ABaseCharacter : public AMinerCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
	ABaseCharacter();

public:
	/**
	* Input Functions
	*/

	// Overrided functions for jumping. Don't need UFunction because it already has on MinerCharacter
	virtual void DoJumpStart() override;
	virtual void DoJumpEnd() override;

	/** Handles start right input */
	UFUNCTION(BlueprintCallable, Category = "Input") 
	void DoStartRightClick();

	/** Handles stop right input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoStopRightClick();

	/** Handles start left input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoStartLeftClick();

	/** Handles stop left input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoStopLeftClick();

	/** Handles start sprint input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoStartSprint();

	/** Handles end sprint input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoEndSprint();

	/** Handles start crouch input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoStartCrouch();

	/** Handles end crouch input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoEndCrouch();

	/** Handles switch weapon input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoSwitchItem();

	///** Attaches a weapon's meshes to the owner */
	//void AttachItemMeshes(ABaseItem* Item);

	///** Plays the firing montage for the weapon */
	//void PlayLeftClickMontage(UAnimMontage* Montage);

	///** Plays the firing montage for the weapon */
	//void PlayRightClickMontage(UAnimMontage* Montage);

	///** Applies weapon recoil to the owner */
	//void AddItemRecoil(float Recoil);

	///** Calculates and returns the aim location for the weapon */
	//FVector GetItemTargetLocation();

	///** Gives a weapon of this class to the owner */
	//void AddItemClass(const TSubclassOf<ABaseItem>& ItemClass);

	///** Activates the passed weapon */
	//void OnItemActivated(ABaseItem* Item);

	///** Deactivates the passed item */
	//void OnItemDeactivated(ABaseItem* Item);

	///** Notifies the owner that the weapon cooldown has expired and it's ready to use again */
	//void OnItemReuse();


	/**
	* Defaults
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	float DefaultWalkSpeed = 600.0f;	// Default walk speed for the character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	float DefaultSprintSpeed = 1000.0f;	// Default sprint speed for the character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	float DefaultJumpHeight = 420.0f;	// Default jump height for the character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	float DefaultSprintJumpHeight = 600.0f;	// Default jump height when sprinting for the character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	float DefaultCrouchSpeed = 300.0f;	// Default crouch speed for the character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defaults")
	float DefaultCrouchHeight = 40.0f;	// Default crouch height for the character

protected:
	/** Input actions */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* RightClickAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SwitchItemAction;

	/** 
	* Gameplay Action Stuff (official terminology)
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/** Get Ability System Component and Return it (when function called) */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

	/** Attribute Set for this character */
	UPROPERTY(BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	const class UBaseCharacterAttributeSet* AttributeSet;	// Have to put the class thing here for some reason

	/** Other actions like movement and jumping are in the parent class, AMinerCharacter */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;	// List of abilities to grant to the character

	/**
	* Inventory
	*/

	/** Sockets (for items putting in hands) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	FName LeftHandSocket = FName("HandGrip_L");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	FName RightHandSocket = FName("HandGrip_R");

	/** List of items picked up by the character */
	TArray<ABaseItem*> OwnedItems;

	/** Item currently equipped and ready to use */
	TObjectPtr<ABaseItem> CurrentItem;

	/**
	* Overrides
	*/

	virtual void BeginPlay() override;	// Called when the game starts and the character is spawned
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;	// Set up input action bindings
	
	/**
	* Helpers
	*/

	/** Returns true if the character already owns a weapon of the given class */
	AShooterWeapon* FindWeaponOfType(TSubclassOf<ABaseItem> WeaponClass) const;

	/**Grant abilities*/
	void GrantAbilities();
};
