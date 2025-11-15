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

	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMeshComponent"));
	DynamicMeshComponent->SetMobility(EComponentMobility::Movable);
	DynamicMeshComponent->SetGenerateOverlapEvents(false);
	DynamicMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	DynamicMeshComponent->CollisionType = ECollisionTraceFlag::CTF_UseDefault;
	DynamicMeshComponent->SetComplexAsSimpleCollisionEnabled(true);
	
	DynamicMeshComponent->SetEnableGravity(false);

	DynamicMeshComponent->SetMaterial(0, UMaterial::GetDefaultMaterial(MD_Surface));		// is this necessary?

	SetRootComponent(DynamicMeshComponent);
}

void AWorldLandscape::BeginPlay()
{
	Super::BeginPlay();

	DynamicMesh = AllocateComputeMesh();
	DynamicMeshComponent->RegisterComponent();
	DynamicMeshComponent->InitializeComponent();
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

	const float TmpHalfSize = 1000.f;
	const float HeightScale = 100.0f;
	const int NumPointsPerLine = FMath::FloorToInt((TmpHalfSize * 2.0f) / Resolution) + 1;

	// reserve - use multiplication, not XOR
	TArray<int32> Verticies;
	int64 ReserveCount = (int64)NumPointsPerLine * (int64)NumPointsPerLine;
	if (ReserveCount > TNumericLimits<int32>::Max()) ReserveCount = TNumericLimits<int32>::Max();
	Verticies.Reserve((int32)ReserveCount);

	// Use EditMesh so UDynamicMesh broadcasts change events correctly
	DynamicMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& Mesh)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Ran correctly");

		Mesh.Clear();

		// create vertices in row-major order: x changes fastest
		for (int iy = 0; iy < NumPointsPerLine; ++iy)
		{
			float y = -TmpHalfSize + iy * Resolution;
			for (int ix = 0; ix < NumPointsPerLine; ++ix)
			{
				float x = -TmpHalfSize + ix * Resolution;

				// sample noise (FastNoiseLite's GetNoise expects floats)
				float h = 0.0f;
				if (Noise)
				{
					h = Noise->GetNoise(x, y) * HeightScale;
				}

				// create vertex and remember its index
				int32 NewVertID = Mesh.AppendVertex(FVector3d(x, y, h));
				Verticies.Add(NewVertID);
			}
		}

		// create two triangles for each quad in the grid
		for (int iy = 0; iy < NumPointsPerLine - 1; ++iy)
		{
			for (int ix = 0; ix < NumPointsPerLine - 1; ++ix)
			{
				const int idx00 = ix + iy * NumPointsPerLine;             // this row/col
				const int idx10 = (ix + 1) + iy * NumPointsPerLine;       // right
				const int idx01 = ix + (iy + 1) * NumPointsPerLine;       // below
				const int idx11 = (ix + 1) + (iy + 1) * NumPointsPerLine; // below-right

				int v00 = Verticies[idx00];
				int v10 = Verticies[idx10];
				int v01 = Verticies[idx01];
				int v11 = Verticies[idx11];

				// First triangle (top-left, bottom-left, bottom-right)
				Mesh.AppendTriangle(v00, v01, v11);

				// Second triangle (top-left, bottom-right, top-right)
				Mesh.AppendTriangle(v00, v11, v10);
			}
		}
	}); // end EditMesh lambda

	DynamicMeshComponent->NotifyMeshUpdated();
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
