// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldLandscape.h"
#include "MinerGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "DynamicMeshBuilder.h"
#include "DynamicMesh/DynamicMesh3.h"	
#include "DynamicMesh/DynamicMeshAttributeSet.h"	
#include "DynamicMesh/MeshNormals.h"	

/*
* CONSTRUCTOR
*/

AWorldLandscape::AWorldLandscape()
{
	// Set variables
	GameMode = Cast<AMinerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

/*
* Overrides
*/

void AWorldLandscape::BeginPlay()
{
	Super::BeginPlay();

	/*
	* Set noise parameters
	*/

	Noise->SetSeed(GameMode->Seed);
	Noise->SetFrequency(NoiseFrequencey);
	Noise->SetNoiseType(StaticCast<FastNoiseLite::NoiseType>(NoiseNoiseType));
	Noise->SetFractalType(StaticCast<FastNoiseLite::FractalType>(NoiseFractalType));
}

/*
* Helper Functions
*/

void AWorldLandscape::LoadTerrainFromSave()
{

}
