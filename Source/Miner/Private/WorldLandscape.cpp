// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldLandscape.h"
#include "MinerGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "DynamicMeshBuilder.h"
#include "DynamicMesh/DynamicMesh3.h"	
#include "DynamicMesh/DynamicMeshAttributeSet.h"	
#include "DynamicMesh/MeshNormals.h"	

AWorldLandscape::AWorldLandscape()
{
	// Set variables
	GameMode = Cast<AMinerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	// Set noise parameters
	//Noise->SetSeed(GameMode->Seed);
	//Noise->SetFrequency(NoiseFrequencey);
	//Noise->SetNoiseType(NoiseNoiseType);
	//Noise->SetFractalType(NoiseFractalType);


}

void AWorldLandscape::LoadTerrainFromSave()
{

}
