// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinerGamemode.h"	// Game mode reference
#include "DynamicMeshActor.h"	// Dynamic mesh stuff
#include "ThirdParty/FastNoiseLite.h"	// Noise generation library
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


protected:
	/**
	* Overriden functions
	*/

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	* Helper functions
	*/

	void GenerateTerrain(); // Generate terrain based on noise
	void LoadTerrainFromSave(); // Load terrain from save file

	/**
	*Other Variables
	*/

	/**References to other classes*/
	TObjectPtr<AMinerGameMode> GameMode;

	/**Dynamic mesh variables*/
	TObjectPtr<UDynamicMeshComponent> DynamicMeshComponent;
	FDynamicMesh3* DynamicMesh = nullptr;

private:
	/**Noise parameters*/
	FastNoiseLite* Noise = nullptr;	// The actual noise variable

	UPROPERTY(EditAnywhere, Category = "World Gen")
	float NoiseScale = 0.1f;	// Scale of the noise
	UPROPERTY(EditAnywhere, Category="World Gen")
	float NoiseAmplitude = 20.0f;
	UPROPERTY(EditAnywhere, Category = "World Gen")
	float NoiseFrequencey = 0.03f;
	UPROPERTY(EditAnywhere, Category = "World Gen")
	/**
	* 1 = NoiseType_OpenSimplex2
	* 2 = NoiseType_OpenSimplex2S
	* 3 = NoiseType_Cellular
	* 4 = NoiseType_Perlin
	* 5 = NoiseType_ValueCubic
	* 6 = NoiseType_Value
	*/
	int NoiseNoiseType = FastNoiseLite::NoiseType_Perlin;
	UPROPERTY(EditAnywhere, Category = "World Gen")
	/**
	* 1 = FractalType_None
	* 2 = FractalType_FBm
	* 3 = FractalType_Ridged
	* 4 = FractalType_PingPong
	* 5 = FractalType_DomainWarpProgressive
	* 6 = FractalType_DomainWarpIndependent
	*/
	int NoiseFractalType = FastNoiseLite::FractalType_FBm;
};
