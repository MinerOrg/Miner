// Copyright Schuyler Zheng. All rights reserved.

#include "WorldLandscape.h"
#include "Engine/CollisionProfile.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "FastNoiseLite.h"
#include "WorldGameMode.h"
#include "WorldGenerationRunnable.h"
#include "Async/Async.h"

DEFINE_LOG_CATEGORY(LogLandscape);

AWorldLandscape::AWorldLandscape()
{
	bReplicates = false;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	DynamicMeshComponent = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("DynamicMeshComponent"));
	DynamicMeshComponent->SetMobility(EComponentMobility::Movable);
	DynamicMeshComponent->SetGenerateOverlapEvents(false);
	DynamicMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	DynamicMeshComponent->CollisionType = ECollisionTraceFlag::CTF_UseDefault;
	DynamicMeshComponent->SetComplexAsSimpleCollisionEnabled(true);
	
	DynamicMeshComponent->SetEnableGravity(false);

	SetRootComponent(DynamicMeshComponent);

	LastPlayerLocation = FVector::ZeroVector;
	LocalClientPawn = nullptr;
	WorldGenerationRunnable = nullptr;

	ApplyTerrainDataDelegate.AddUObject(this, &AWorldLandscape::GenerateTerrain);
}

void AWorldLandscape::BeginPlay()
{
	Super::BeginPlay();

	checkf(LandscapeData.ChunkDistance != 0, TEXT("Chunk distance cannot be 0"));

	// Get local client pawn
	checkf(IsValid(LocalClientPawn = GetWorld()->GetFirstPlayerController()->GetPawn()), TEXT("Local Pawn was bad"));
	LastPlayerLocation = LocalClientPawn->GetActorLocation();

	DynamicMesh = AllocateComputeMesh();
	DynamicMeshComponent->SetDynamicMesh(DynamicMesh);
	DynamicMeshComponent->SetMaterial(0, LandscapeData.LandscapeMaterials.GrassMaterial);

	// Create the thread that generates the vertex locations
	WorldGenerationRunnable = new FWorldGenerationRunnable(this, GetWorld());

	// Pre-allocate vertex locations array
	const int NumPointsPerLine = FMath::FloorToInt((LandscapeData.RenderDistance * 2.0f) / LandscapeData.Resolution) + 1;
	int64 ReserveCount = (int64)NumPointsPerLine * (int64)NumPointsPerLine;
	if (ReserveCount > TNumericLimits<int32>::Max()) ReserveCount = TNumericLimits<int32>::Max();
	GeneratedVertexLocations.Reserve((int32)ReserveCount);

	SetupNoise();

	// Generate the mesh (last step)
	WorldGenerationRunnable->bGenerate = true;
	while (WorldGenerationRunnable->bGenerate) { FPlatformProcess::Sleep(0.1f); } // Wait for it to finish
}

void AWorldLandscape::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	checkf(IsValid(LocalClientPawn), TEXT("Client Pawn bad"));
	FVector LocalClientPawnLocation = LocalClientPawn->GetActorLocation();

	// If the player has moved out of bounds, make the mesh follow them. (No Z check for now) (make sure that this runs last because if it is generating it will return)
	if (FMath::RoundToInt(LastPlayerLocation.X / LandscapeData.ChunkDistance) * LandscapeData.ChunkDistance != FMath::RoundToInt(LocalClientPawnLocation.X / LandscapeData.ChunkDistance) * LandscapeData.ChunkDistance || FMath::RoundToInt(LastPlayerLocation.Y / LandscapeData.ChunkDistance) * LandscapeData.ChunkDistance != FMath::RoundToInt(LocalClientPawnLocation.Y / LandscapeData.ChunkDistance) * LandscapeData.ChunkDistance /* || FMath::RoundToInt(LastPlayerLocation.Z / ChunkDistance) * ChunkDistance != LocalClientPawnLocation.Z */) {
		if (WorldGenerationRunnable->bGenerate) { return; }
		
		LastPlayerLocation = LocalClientPawnLocation;

		// Request mesh data to be generated (apply the mesh later)
		WorldGenerationRunnable->bGenerate = true;
	}
}

void AWorldLandscape::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FreeAllComputeMeshes();
	delete WorldGenerationRunnable;
}

void AWorldLandscape::SetupNoise()
{
	// Get the seed from the gamemode
	if (IsValid(UGameplayStatics::GetGameMode(GetWorld())))    LandscapeData.Seed = CastChecked<AWorldGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->Seed;
	else LandscapeData.Seed = 1337;

	// Set noise parameters
	SetNoiseParameters(BasicLandNoise, BasicLandNoiseSettings);
	SetNoiseParameters(PlateTectonicsNoise, PlateTectonicsNoiseSettings);
}

void AWorldLandscape::GenerateTerrain()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(GenerateTerrain);

	check(IsValid(DynamicMeshComponent));
	check(IsValid(DynamicMesh));
	check(BasicLandNoise);

	DynamicMesh->InitializeMesh();
	DynamicMeshComponent->SetMaterial(0, LandscapeData.LandscapeMaterials.GrassMaterial);
	
	// EditMesh > just using notifymesh because more safe
	DynamicMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& Mesh) {
		Mesh = *WorldGenerationRunnable->DynamicMesh->GetMeshPtr();

		// Final validity checks
		ensure(Mesh.CheckValidity(ValidityOptions, ValidityCheckFailMode));
		ensure(Mesh.IsCompact());
	});

	DynamicMeshComponent->SetMaterial(0, LandscapeData.LandscapeMaterials.GrassMaterial);
}

void AWorldLandscape::SetNoiseParameters(TObjectPtr<FastNoiseLite>& NoiseObject, const FNoiseSettings& NoiseSettings)
{
	NoiseObject = new FastNoiseLite();

	NoiseObject->SetSeed(LandscapeData.Seed);
	NoiseObject->SetFrequency(NoiseSettings.Frequency);
	NoiseObject->SetNoiseType(static_cast<FastNoiseLite::NoiseType>(NoiseSettings.NoiseType.GetValue()));
	NoiseObject->SetRotationType3D(static_cast<FastNoiseLite::RotationType3D>(NoiseSettings.RotationType3D.GetValue()));
	NoiseObject->SetFractalType(static_cast<FastNoiseLite::FractalType>(NoiseSettings.FractalType.GetValue()));
	NoiseObject->SetFractalOctaves(NoiseSettings.FractalOctaves);
	NoiseObject->SetFractalLacunarity(NoiseSettings.FractalLacunarity);
	NoiseObject->SetFractalGain(NoiseSettings.FractalGain);
	NoiseObject->SetFractalWeightedStrength(NoiseSettings.FractalWeightedStrength);
	NoiseObject->SetFractalPingPongStrength(NoiseSettings.FractalPingPongStrength);
	NoiseObject->SetCellularDistanceFunction(static_cast<FastNoiseLite::CellularDistanceFunction>(NoiseSettings.CellularDistanceFunction.GetValue()));
	NoiseObject->SetCellularReturnType(static_cast<FastNoiseLite::CellularReturnType>(NoiseSettings.CellularReturnType.GetValue()));
	NoiseObject->SetCellularJitter(NoiseSettings.CellularJitter);
	NoiseObject->SetDomainWarpType(static_cast<FastNoiseLite::DomainWarpType>(NoiseSettings.DomainWarpType.GetValue()));
	NoiseObject->SetDomainWarpAmp(NoiseSettings.DomainWarpAmp);
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
