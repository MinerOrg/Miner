// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbilities.generated.h"

UENUM(BlueprintType)
enum class EAbilitiesIndex : uint8
{
	SprintAbility	UMETA(DisplayName = "Sprint Ability"),
	JumpAbility	UMETA(DisplayName = "Jump Ability"),
	CrouchAbility	UMETA(DisplayName = "Crouch Ability"),
	LeftClickAbility	UMETA(DisplayName = "Left Click Ability"),
	RightClickAbility	UMETA(DisplayName = "Right Click Ability"),

};
