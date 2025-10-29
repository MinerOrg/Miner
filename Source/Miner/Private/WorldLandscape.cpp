// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldLandscape.h"
#include "MinerGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "DynamicMesh/DynamicMesh3.h"	
#include "DynamicMesh/MeshNormals.h"
#include "GeometryTypes.h"

/*
* CONSTRUCTOR
*/

AWorldLandscape::AWorldLandscape()
{
	/**
	* Set Variables
	*/

	/**Set noise*/
	Noise = new FastNoiseLite();	// Noise variable becomes safe to use
}

/*
* Overrides
*/

void AWorldLandscape::BeginPlay()
{
	Super::BeginPlay();

	DynamicMeshComponent = GetDynamicMeshComponent();
	checkf(IsValid(DynamicMeshComponent), TEXT("Dynamic Mesh Component Invalid"));

	// Set up collision here, after DynamicMeshComponent is valid
	DynamicMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DynamicMeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	DynamicMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);

	/** Start the process of getting the FDynamicMesh3 */
	checkf(IsValid(DynamicMeshComponent), TEXT("Dynamic Mesh Component Invalid"));
	UDynamicMesh* DynamicMeshTemp = DynamicMeshComponent->GetDynamicMesh();
	checkf(IsValid(DynamicMeshTemp), TEXT("DynamicMeshTemp (UDynamicMesh*) was invalid"));
	DynamicMesh = DynamicMeshTemp->GetMeshPtr();

	/**Get references to other stuff*/
	GameMode = Cast<AMinerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	/** Set noise parameters*/
	checkf(Noise, TEXT("Noise was still null"));	// Check if noise is valid before setting parameters
	Noise->SetSeed(GameMode->Seed);
	Noise->SetFrequency(NoiseFrequencey);
	Noise->SetNoiseType(static_cast<FastNoiseLite::NoiseType>(NoiseNoiseType));
	Noise->SetFractalType(static_cast<FastNoiseLite::FractalType>(NoiseFractalType));

	// Temp
	GenerateTerrain();
}

void AWorldLandscape::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/**Clean up fastnoiselite stuff*/
	check(Noise);	// Check if noise is valid

	delete Noise;
	Noise = nullptr;
}

/*
* Helper Functions
*/

void AWorldLandscape::GenerateTerrain()
{
	/**CHECK IT ALL*/
	check(IsValid(DynamicMeshComponent));
	//checkf(!DynamicMesh->CheckValidity(ValidityOptions), TEXT("Dynamic Mesh Check failed.")); // Don't need because terrain isn't generated yet

	DynamicMesh->Clear();

	const int Subdivisions = 10; // or higher

	const float Size = 10000.0f;

	const int32 NumX = Subdivisions + 1;
	const int32 NumY = Subdivisions + 1;

	const float Step = Size / Subdivisions;
	const float HalfSize = Size / 2.0f;

	// Generate vertices
	for (int32 y = 0; y < NumY; ++y)
	{
		for (int32 x = 0; x < NumX; ++x)
		{
			float vx = -HalfSize + x * Step;
			float vy = -HalfSize + y * Step;
			float vz = 0.0f;
			DynamicMesh->AppendVertex(FVector3d(vx, vy, vz));
		}
	}

	// Generate triangles
	for (int32 y = 0; y < Subdivisions; ++y)
	{
		for (int32 x = 0; x < Subdivisions; ++x)
		{
			int32 v0 = x + y * NumX;
			int32 v1 = (x + 1) + y * NumX;
			int32 v2 = x + (y + 1) * NumX;
			int32 v3 = (x + 1) + (y + 1) * NumX;

			// Two triangles per grid square
			DynamicMesh->AppendTriangle(v0, v2, v1);
			DynamicMesh->AppendTriangle(v1, v2, v3);
		}
	}

	//// Compute normals
	//UE::Geometry::FMeshNormals Normals(&*DynamicMesh);
	//Normals.ComputeVertexNormals();
	//Normals.CopyToOverlay(DynamicMesh->Attributes()->PrimaryNormals());

	DynamicMeshComponent->NotifyMeshUpdated();
	
}

void AWorldLandscape::LoadTerrainFromSave()
{

}
