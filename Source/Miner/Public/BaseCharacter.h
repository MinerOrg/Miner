// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinerCharacter.h"
#include "AbilitySystemInterface.h"
#include "BaseCharacterAttributeSet.h"
#include "BaseCharacter.generated.h"

class ABaseItem;
class AShooterWeapon;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	const UBaseCharacterAttributeSet* AttributeSet;

	/** Other actions like movement and jumping are in the parent class, AMinerCharacter */

	/** Sockets (for items putting in hands) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	FName LeftHandSocket = FName("HandGrip_L");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
	FName RightHandSocket = FName("HandGrip_R");

	/**
	* Inventory
	*/

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
};
