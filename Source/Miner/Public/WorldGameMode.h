// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinerGameMode.h"
#include "WorldGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MINER_API AWorldGameMode : public AMinerGameMode
{
	GENERATED_BODY()
	
public:
	AWorldGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Seed = 6767;
};
