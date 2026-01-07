// Copyright Schuyler Zheng. All Rights Reserved.

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
