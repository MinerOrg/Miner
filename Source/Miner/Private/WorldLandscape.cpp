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
	/**Start the process of getting the FDynamicMesh3*/
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
	int TempMax = 0;

	/**CHECK IT ALL*/
	check(IsValid(DynamicMeshComponent));
	//checkf(DynamicMesh->CheckValidity(FValidityOptions::Permissive(), EValidityFailMode::Check), TEXT("Dynamic Mesh Check failed."));	// Cooked

	// Generation loop
	for (int Vertex : DynamicMesh->VertexIndicesItr()) {
		if (TempMax > 100) {
			break;
		}

		FVector3d Pos = DynamicMesh->GetVertex(Vertex);	// Get the position of the vertex

		// Sample 2D Perlin noise at XY, you can do 3D by passing FVector3d
		double Displace = Noise->GetNoise(Pos.X, Pos.Y, Pos.Z) * NoiseScale;

		checkf(!DynamicMesh, TEXT("Dynamic Mesh bad"));	// Check if dynamic mesh isn't a nullptr (used in highrisk areas)

		/**Extra checks for line 89*/
		checkf(!DynamicMesh->IsVertex(Vertex), TEXT("Vertex bad"));

		// Get the normals so we can push along it
		FVector3d Normal = DynamicMesh->HasVertexNormals()
			? (FVector3d)DynamicMesh->GetVertexNormal(Vertex)	// If true do this
			: FVector3d(0, 0, 1);	// Else do this

		checkf(DynamicMesh != nullptr, TEXT("Dynamic Mesh invalid"));	// Check if dynamic mesh isn't a nullptr (used in highrisk areas)

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
