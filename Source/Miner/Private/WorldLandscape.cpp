// Copyright me

#include "WorldLandscape.h"
#include "Engine/CollisionProfile.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "FastNoiseLite.h"
#include "WorldGameMode.h"

AWorldLandscape::AWorldLandscape()
{
	DynamicMeshComponent = CreateDefaultSubobject<UOctreeDynamicMeshComponent>(TEXT("DynamicMeshComponent"));
	DynamicMeshComponent->SetMobility(EComponentMobility::Movable);
	DynamicMeshComponent->SetGenerateOverlapEvents(false);
	DynamicMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	//DynamicMeshComponent->CollisionType = ECollisionTraceFlag::CTF_UseDefault;
	DynamicMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DynamicMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);

	DynamicMeshComponent->SetSimulatePhysics(true);
	DynamicMeshComponent->SetEnableGravity(false);

	DynamicMeshComponent->SetMaterial(0, UMaterial::GetDefaultMaterial(MD_Surface));		// is this necessary?	(I don't know, you guys made the engine) - Me

	SetRootComponent(DynamicMeshComponent);
}

void AWorldLandscape::BeginPlay()
{
	Super::BeginPlay();

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
	Noise = FastNoiseLite();

	Seed = Cast<AWorldGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->Seed;

	Noise.SetSeed(Seed);
	Noise.SetFrequency(Frequency);
	Noise.SetNoiseType(static_cast<FastNoiseLite::NoiseType>(NoiseType.GetValue()));
	Noise.SetRotationType3D(static_cast<FastNoiseLite::RotationType3D>(RotationType3D.GetValue()));
	Noise.SetFractalType(static_cast<FastNoiseLite::FractalType>(FractalType.GetValue()));
	Noise.SetFractalOctaves(FractalOctaves);
	Noise.SetFractalLacunarity(FractalLacunarity);
	Noise.SetFractalGain(FractalGain);
	Noise.SetFractalWeightedStrength(FractalWeightedStrength);
	Noise.SetFractalPingPongStrength(FractalPingPongStrength);
	Noise.SetCellularDistanceFunction(static_cast<FastNoiseLite::CellularDistanceFunction>(CellularDistanceFunction.GetValue()));
	Noise.SetCellularReturnType(static_cast<FastNoiseLite::CellularReturnType>(CellularReturnType.GetValue()));
	Noise.SetCellularJitter(CellularJitter);
	Noise.SetDomainWarpType(static_cast<FastNoiseLite::DomainWarpType>(DomainWarpType.GetValue()));
	Noise.SetDomainWarpAmp(DomainWarpAmp);
}

void AWorldLandscape::GenerateTerrain()
{
	checkf(IsValid(DynamicMeshComponent), TEXT("Dynamic Mesh Component was bad"));
	checkf(DynamicMesh.IsValidLowLevel(), TEXT("Dynamic Mesh was bad"));
	
	DynamicMesh.GetMeshPtr()->Clear();

	float TmpSize = 10.f;
	float HeightScale = 10;

	for (float x = -TmpSize; x <= TmpSize; x += Resolution) {
		for (float y = -TmpSize; y <= TmpSize; y += Resolution) {
			float NoiseValue = Noise.GetNoise(x, y) * HeightScale;
			FVector V0 = FVector(x, y, NoiseValue);
			FVector V1 = FVector(x + Resolution, y, Noise.GetNoise((x + Resolution), y) * HeightScale);
			FVector V2 = FVector(x, y + Resolution, Noise.GetNoise(x, (y + Resolution)) * HeightScale);
			FVector V3 = FVector(x + Resolution, y + Resolution, Noise.GetNoise((x + Resolution), (y + Resolution)) * HeightScale);
			int32 V0ID = DynamicMesh.GetMeshPtr()->AppendVertex(V0);
			int32 V1ID = DynamicMesh.GetMeshPtr()->AppendVertex(V1);
			int32 V2ID = DynamicMesh.GetMeshPtr()->AppendVertex(V2);
			int32 V3ID = DynamicMesh.GetMeshPtr()->AppendVertex(V3);

			DynamicMesh.GetMeshPtr()->AppendTriangle(V0ID, V2ID, V1ID);
			DynamicMesh.GetMeshPtr()->AppendTriangle(V1ID, V2ID, V3ID);
		}
	}

	DynamicMeshComponent->NotifyMeshUpdated();
	DynamicMeshComponent->UpdateCollisionProfile();
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
