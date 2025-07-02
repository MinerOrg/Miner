// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinerCharacter.h"
#include "BaseItem.h"
#include "ShooterWeaponHolder.h"
#include "BaseCharacter.generated.h"

/**
 * Base Character class used for player character, enemies, etc.
 */
UCLASS()
class MINER_API ABaseCharacter : public AMinerCharacter
{
	GENERATED_BODY()
	
	ABaseCharacter();

public:
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

	/** Handles switch weapon input */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoSwitchItem();

	//~Begin IShooterWeaponHolder interface

	/** Attaches a weapon's meshes to the owner */
	void AttachItemMeshes(ABaseItem* Item);

	/** Plays the firing montage for the weapon */
	void PlayLeftClickMontage(UAnimMontage* Montage);

	/** Plays the firing montage for the weapon */
	void PlayRightClickMontage(UAnimMontage* Montage);

	/** Applies weapon recoil to the owner */
	void AddItemRecoil(float Recoil);

	/** Updates the weapon's HUD with the current ammo count */
	void UpdateItemHUD(int32 CurrentCount, int32 MaxStackSize);

	/** Calculates and returns the aim location for the weapon */
	FVector GetItemTargetLocation();

	/** Gives a weapon of this class to the owner */
	void AddItemClass(const TSubclassOf<ABaseItem>& ItemClass);

	/** Activates the passed weapon */
	void OnItemActivated(ABaseItem* Item);

	/** Deactivates the passed item */
	void OnItemDeactivated(ABaseItem* Item);

	/** Notifies the owner that the weapon cooldown has expired and it's ready to use again */
	void OnItemReuse();

	//~End IShooterWeaponHolder interface

protected:
	/** Input actions */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LeftClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* RightClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SwitchItemAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

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

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;	// Set up input action bindings
	
	/**
	* Helpers
	*/

	/** Returns true if the character already owns a weapon of the given class */
	AShooterWeapon* FindWeaponOfType(TSubclassOf<ABaseItem> WeaponClass) const;
};
