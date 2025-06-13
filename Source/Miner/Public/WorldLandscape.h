// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMeshActor.h"	// Dynamic mesh stuff
#include "DynamicMeshBuilder.h"    // Dynamic mesh stuff
#include "FastNoiseLite.h"	// Noise generation library
#include "WorldLandscape.generated.h"	// Generated header

/**
 * 
 */
UCLASS()
class MINER_API AWorldLandscape : public ADynamicMeshActor
{
	GENERATED_BODY()
	
	AWorldLandscape();

public:
	
	TObjectPtr<FastNoiseLite> Noise;

protected:
	void LoadTerrainFromSave();

private:
	// Noise parameters
	UPROPERTY(EditAnywhere, Category = "Terrain")
	float NoiseFrequencey = 0.03f;
	/* Turns out you can't find the noise type because it is not just one type, so I'll just set it to perlin
	UPROPERTY(EditAnywhere, Category = "Terrain")
	FastNoiseLite::NoiseType NoiseType = FastNoiseLite::NoiseType_Perlin;
	UPROPERTY(EditAnywhere, Category = "Terrain")
	FastNoiseLite::FractalType NoiseFractalType = FastNoiseLite::FractalType_FBm;*/

	AMinerGameMode* GameMode;

	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;
	
};
