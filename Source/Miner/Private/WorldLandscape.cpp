// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldLandscape.h"
#include "MinerGamemode.h"
#include "Kismet/GameplayStatics.h"
#include "DynamicMesh/DynamicMesh3.h"	
#include "DynamicMesh/MeshNormals.h"

/*
* CONSTRUCTOR
*/

AWorldLandscape::AWorldLandscape()
{
	/**
	* Set Variables
	*/
	
	/**Set dynamic mesh variables*/
	DynamicMeshComponent = GetDynamicMeshComponent();
	DynamicMesh = reinterpret_cast<FDynamicMesh3*>(DynamicMeshComponent->GetDynamicMesh());
	
	/**Get references to other stuff*/
	GameMode = Cast<AMinerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	Noise = new FastNoiseLite();	// Noise variable becomes safe to use

	check(Noise);	// CHECK 
	check(IsValid(GameMode));	// CHECK (Note: THIS IS THE PROBLEM)

	/** Set noise parameters*/
	Noise->SetSeed(GameMode->Seed);
	Noise->SetFrequency(NoiseFrequencey);
	Noise->SetNoiseType(static_cast<FastNoiseLite::NoiseType>(NoiseNoiseType));
	Noise->SetFractalType(static_cast<FastNoiseLite::FractalType>(NoiseFractalType));

}

/*
* Overrides
*/

void AWorldLandscape::BeginPlay()
{
	Super::BeginPlay();

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
	int TempMax = 0;

	/**CHECK IT ALL*/
	check(IsValid(DynamicMeshComponent));
	check(DynamicMesh);
	check(Noise);
	check(NoiseAmplitude);
	check(NoiseScale);

	// Generation loop
	for (int Vertex : DynamicMesh->VertexIndicesItr()) {
		if (TempMax > 100) {
			break;
		}
		FVector3d Pos = DynamicMesh->GetVertex(Vertex);

		// Sample 2D Perlin noise at XY, you can do 3D by passing FVector3d
		double Displace = Noise->GetNoise(Pos.X, Pos.Y, Pos.Z) * NoiseScale;

		// Check variable
		check(Vertex);
		check(Displace);
		check(NoiseAmplitude);

		// Get the normals so we can push along it
		FVector3d Normal = DynamicMesh->HasVertexNormals()
			? (FVector3d)DynamicMesh->GetVertexNormal(Vertex)	// If true do this
			: FVector3d(0, 0, 1);	// Else do this

		// Displace
		DynamicMesh->SetVertex(Vertex, Pos + Normal * (Displace * NoiseAmplitude));

		TempMax++;
	}

	/**Recalculate normals*/
	UE::Geometry::FMeshNormals Normals(&*DynamicMesh);
	Normals.ComputeVertexNormals();
	Normals.CopyToOverlay(DynamicMesh->Attributes()->PrimaryNormals());

	DynamicMeshComponent->NotifyMeshUpdated();
}

void AWorldLandscape::LoadTerrainFromSave()
{

}
