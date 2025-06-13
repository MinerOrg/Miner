// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinerGamemode.h"	// Game mode reference
#include "DynamicMeshActor.h"	// Dynamic mesh stuff
#include "DynamicMeshBuilder.h"    // Dynamic mesh stuff
#include "FastNoiseLite.h"	// Noise generation library
#include "WorldLandscape.generated.h"	// Generated header

/**
 * TERRAIN CLASS
 */
UCLASS()
class MINER_API AWorldLandscape : public ADynamicMeshActor
{
	GENERATED_BODY()
	
	AWorldLandscape();

public:
	
	FastNoiseLite* Noise;

protected:
	// Helper Functions
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

	// References to other stuff
	TObjectPtr<AMinerGameMode> GameMode = nullptr;

	// Dynamic mesh
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;
	
};
