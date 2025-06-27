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
	
	DynamicMeshComponent = GetDynamicMeshComponent();
	DynamicMesh = reinterpret_cast<FDynamicMesh3*>(DynamicMeshComponent->GetDynamicMesh());
	
	/**Get references to other stuff*/
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
	Noise->SetNoiseType(static_cast<FastNoiseLite::NoiseType>(NoiseNoiseType));
	Noise->SetFractalType(static_cast<FastNoiseLite::FractalType>(NoiseFractalType));

	// Temp
	GenerateTerrain();
}

/*
* Helper Functions
*/

void AWorldLandscape::GenerateTerrain()
{
	// Generation loop
	for (int Vertex : DynamicMesh->VertexIndicesItr()) {
		FVector3d Pos = DynamicMesh->GetVertex(Vertex);

		// Sample 2D Perlin noise at XY, you can do 3D by passing FVector3d
		double Displace = Noise->GetNoise(Pos.X, Pos.Y, Pos.Z) * NoiseScale;

		// Get the normals so we can push along it
		FVector3d Normal = DynamicMesh->HasVertexNormals()
			? (FVector3d)DynamicMesh->GetVertexNormal(Vertex)	// If true do this
			: FVector3d(0, 0, 1);	// Else do this

		// Displace
		DynamicMesh->SetVertex(Vertex, Pos + Normal * (Displace * NoiseAmplitude));
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
