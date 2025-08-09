// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/UseStamina.h"

UUseStamina::UUseStamina()
{
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UseStamina Gameplay Effect Created")); }
	else { UE_LOG(LogTemp, Warning, TEXT("UseStamina Gameplay Effect Created but GEngine is null")); }
}
