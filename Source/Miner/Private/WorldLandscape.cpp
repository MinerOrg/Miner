// Fill out your copyright notice in the Description page of Project Settings.

#include "MinerGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "DynamicMeshBuilder.h"
#include "DynamicMesh3.h"
#include "DynamicMeshAttributeSet.h"
#include "DynamicMeshNormals.h"
#include "DynamicMeshUVOverlay.h"
#include "WorldLandscape.h"

AWorldLandscape::AWorldLandscape()
{
	// Set variables
	GameMode = Cast<AMinerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// Set noise parameters
	Noise->SetSeed(GameMode->Seed);
	Noise->SetFrequency(NoiseFrequencey);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);


}

void AWorldLandscape::LoadTerrainFromSave()
{

}
