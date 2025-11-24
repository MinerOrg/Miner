// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MINER_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ABasePlayerState();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Replication" )
	float BaseNetUpdateFrequency = 100.0f;
};
