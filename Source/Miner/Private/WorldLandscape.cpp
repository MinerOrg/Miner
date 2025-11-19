// Copyright Schuyler Zheng. All rights reserved.

#include "WorldLandscape.h"
#include "Engine/CollisionProfile.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "FastNoiseLite.h"
#include "WorldGameMode.h"

AWorldLandscape::AWorldLandscape()
{
	bReplicates = true;

	LocalClientPawn = UGameplayStatics::GetPlayerPawn(this, 0);	// Index 0 is only local client???

	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMeshComponent"));
	DynamicMeshComponent->SetMobility(EComponentMobility::Movable);
	DynamicMeshComponent->SetGenerateOverlapEvents(false);
	DynamicMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	DynamicMeshComponent->CollisionType = ECollisionTraceFlag::CTF_UseDefault;
	DynamicMeshComponent->SetComplexAsSimpleCollisionEnabled(true);
	
	DynamicMeshComponent->SetEnableGravity(false);

	DynamicMeshComponent->SetMaterial(0, DefaultLandscapeMaterial);

	SetRootComponent(DynamicMeshComponent);
}

void AWorldLandscape::BeginPlay()
{
	Super::BeginPlay();

	DynamicMesh = AllocateComputeMesh();
	DynamicMeshComponent->SetDynamicMesh(DynamicMesh);

	SetupNoise();
	GenerateTerrain();
}

void AWorldLandscape::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FreeAllComputeMeshes();
}

void AWorldLandscape::SetupNoise()
{
	Noise = new FastNoiseLite();

	if (UGameplayStatics::GetGameMode(GetWorld()) != nullptr)	{ Seed = CastChecked<AWorldGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->Seed;	}
	else { UE_LOG(LogTemp, Warning, TEXT("SetupNoise: GetGameMode() returned null (client). Ensure Seed is replicated via GameState or actor property.")); }

	Noise->SetSeed(Seed);
	Noise->SetFrequency(Frequency);
	Noise->SetNoiseType(static_cast<FastNoiseLite::NoiseType>(NoiseType.GetValue()));
	Noise->SetRotationType3D(static_cast<FastNoiseLite::RotationType3D>(RotationType3D.GetValue()));
	Noise->SetFractalType(static_cast<FastNoiseLite::FractalType>(FractalType.GetValue()));
	Noise->SetFractalOctaves(FractalOctaves);
	Noise->SetFractalLacunarity(FractalLacunarity);
	Noise->SetFractalGain(FractalGain);
	Noise->SetFractalWeightedStrength(FractalWeightedStrength);
	Noise->SetFractalPingPongStrength(FractalPingPongStrength);
	Noise->SetCellularDistanceFunction(static_cast<FastNoiseLite::CellularDistanceFunction>(CellularDistanceFunction.GetValue()));
	Noise->SetCellularReturnType(static_cast<FastNoiseLite::CellularReturnType>(CellularReturnType.GetValue()));
	Noise->SetCellularJitter(CellularJitter);
	Noise->SetDomainWarpType(static_cast<FastNoiseLite::DomainWarpType>(DomainWarpType.GetValue()));
	Noise->SetDomainWarpAmp(DomainWarpAmp);
}

void AWorldLandscape::GenerateTerrain()
{
	checkf(IsValid(DynamicMeshComponent), TEXT("Dynamic Mesh Component was bad"));
	checkf(IsValid(DynamicMesh), TEXT("Dynamic Mesh was bad"));
	
	DynamicMesh->InitializeMesh();

	// EditMesh > just using notifymesh because more safe
	DynamicMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& Mesh) {
		InitialMeshGeneration(Mesh);
		PostGeneration(Mesh);
	});
}

void AWorldLandscape::InitialMeshGeneration(UE::Geometry::FDynamicMesh3& Mesh)
{
	const int NumPointsPerLine = FMath::FloorToInt((TmpHalfSize * 2.0f) / Resolution) + 1;

	TArray<int32> Verticies;
	int64 ReserveCount = (int64)NumPointsPerLine * (int64)NumPointsPerLine;
	if (ReserveCount > TNumericLimits<int32>::Max()) ReserveCount = TNumericLimits<int32>::Max();
	Verticies.Reserve((int32)ReserveCount);

	// create vertices in row-major order: x changes fastest (this makes it faster?)
	for (int iy = 0; iy < NumPointsPerLine; ++iy) {
		float y = -TmpHalfSize + iy * Resolution;

		for (int ix = 0; ix < NumPointsPerLine; ++ix) {
			float x = -TmpHalfSize + ix * Resolution;

			// get noise for height
			checkf(Noise, TEXT("Noise was bad"));
			float h = Noise->GetNoise(x, y) * HeightScale;

			// create vertex and remember its index
			int32 NewVertex = Mesh.AppendVertex(FVector3d(x, y, h));
			check(Mesh.IsVertex(NewVertex));
			Verticies.Add(NewVertex);
		}
	}

	// Create the triangles
	for (int iy = 0; iy < NumPointsPerLine - 1; ++iy) {
		for (int ix = 0; ix < NumPointsPerLine - 1; ++ix) {
			// First triangle (top-left, bottom-left, bottom-right)
			Mesh.AppendTriangle(Verticies[ix + iy * NumPointsPerLine], Verticies[ix + (iy + 1) * NumPointsPerLine], Verticies[(ix + 1) + (iy + 1) * NumPointsPerLine]);

			// Second triangle (top-left, bottom-right, top-right)
			Mesh.AppendTriangle(Verticies[ix + iy * NumPointsPerLine], Verticies[(ix + 1) + (iy + 1) * NumPointsPerLine], Verticies[(ix + 1) + iy * NumPointsPerLine]);
		}
	}
}

void AWorldLandscape::PostGeneration(UE::Geometry::FDynamicMesh3& Mesh)
{
	
}

UDynamicMeshPool* AWorldLandscape::GetComputeMeshPool()
{
	if (DynamicMeshPool == nullptr && bEnableComputeMeshPool)
	{
		DynamicMeshPool = NewObject<UDynamicMeshPool>();
	}
	return DynamicMeshPool;
}


UDynamicMesh* AWorldLandscape::AllocateComputeMesh()
{
	if (bEnableComputeMeshPool)
	{
		UDynamicMeshPool* UsePool = GetComputeMeshPool();
		if (UsePool)
		{
			return UsePool->RequestMesh();
		}
	}

	// if we could not return a pool mesh, allocate a new mesh that isn't owned by the pool
	return NewObject<UDynamicMesh>(this);
}


bool AWorldLandscape::ReleaseComputeMesh(UDynamicMesh* Mesh)
{
	if (bEnableComputeMeshPool && Mesh)
	{
		UDynamicMeshPool* UsePool = GetComputeMeshPool();
		if (UsePool != nullptr)
		{
			UsePool->ReturnMesh(Mesh);
			return true;
		}
	}
	return false;
}


void AWorldLandscape::ReleaseAllComputeMeshes()
{
	UDynamicMeshPool* UsePool = GetComputeMeshPool();
	if (UsePool)
	{
		UsePool->ReturnAllMeshes();
	}
}

void AWorldLandscape::FreeAllComputeMeshes()
{
	UDynamicMeshPool* UsePool = GetComputeMeshPool();
	if (UsePool)
	{
		UsePool->FreeAllMeshes();
	}
}
