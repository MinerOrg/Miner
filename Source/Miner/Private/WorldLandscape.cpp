// Copyright Schuyler Zheng. All rights reserved.

#include "WorldLandscape.h"
#include "Engine/CollisionProfile.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "FastNoiseLite.h"
#include "WorldGameMode.h"
#include "WorldGenerationRunnable.h"

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

	DynamicMeshComponent->SetMaterial(0, DefaultLandscapeMaterial);

	SetRootComponent(DynamicMeshComponent);

	LastPlayerLocation = FVector::ZeroVector;
	LocalClientPawn = nullptr;
	WorldGenRunnable = nullptr;
	WorldGenThread = nullptr;
}

void AWorldLandscape::BeginPlay()
{
	Super::BeginPlay();

	checkf(ChunkDistance != 0, TEXT("Chunk distance cannot be 0"));

	// Get local client pawn
	checkf(IsValid(LocalClientPawn = GetWorld()->GetFirstPlayerController()->GetPawn()), TEXT("Local Pawn was bad"));
	LastPlayerLocation = LocalClientPawn->GetActorLocation();

	DynamicMesh = AllocateComputeMesh();
	DynamicMeshComponent->SetDynamicMesh(DynamicMesh);

	SetupNoise();

	// Create the thread that generates the vertex locations
	WorldGenRunnable = new FWorldGenerationRunnable(this, GetWorld());
	WorldGenThread = FRunnableThread::Create(WorldGenRunnable, TEXT("WorldGenerationThread"));

	GenerateTerrain();
}

void AWorldLandscape::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	checkf(IsValid(LocalClientPawn), TEXT("Client Pawn bad"));
	FVector LocalClientPawnLocation = LocalClientPawn->GetActorLocation();

	// If the player has moved out of bounds, make the mesh follow them. (No Z check for now)
	if (FMath::RoundToInt(LastPlayerLocation.X / ChunkDistance) * ChunkDistance != FMath::RoundToInt(LocalClientPawnLocation.X / ChunkDistance) * ChunkDistance || FMath::RoundToInt(LastPlayerLocation.Y / ChunkDistance) * ChunkDistance != FMath::RoundToInt(LocalClientPawnLocation.Y / ChunkDistance) * ChunkDistance /* || FMath::RoundToInt(LastPlayerLocation.Z / ChunkDistance) * ChunkDistance != LocalClientPawnLocation.Z */) {
		LastPlayerLocation = LocalClientPawnLocation;
		GenerateTerrain();
	}
}

void AWorldLandscape::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FreeAllComputeMeshes();
}

void AWorldLandscape::SetupNoise()
{
	// Make a new noise 
	Noise = new FastNoiseLite();

	// Get the seed from the gamemode
	checkf(IsValid(UGameplayStatics::GetGameMode(GetWorld())), TEXT("Gamemode was bad"));
	Seed = CastChecked<AWorldGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->Seed;

	// Set noise parameters
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
	TRACE_CPUPROFILER_EVENT_SCOPE(GenerateTerrain);

	checkf(IsValid(DynamicMeshComponent), TEXT("Dynamic Mesh Component was bad"));
	checkf(IsValid(DynamicMesh), TEXT("Dynamic Mesh was bad"));
	checkf(Noise, TEXT("Noise was bad"));
	
	DynamicMesh->InitializeMesh();

	// EditMesh > just using notifymesh because more safe
	DynamicMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& Mesh) {
		InitialMeshGeneration(Mesh);
		PostGeneration(Mesh);

		// Final validity checks
		ensureMsgf(Mesh.CheckValidity(ValidityOptions, ValidityCheckFailMode), TEXT("Mesh was not valid"));
		ensureMsgf(Mesh.IsCompact(), TEXT("Mesh had gaps (was not compact)"));
	});
}

void AWorldLandscape::InitialMeshGeneration(UE::Geometry::FDynamicMesh3& Mesh)
{
	// Won't generate in a non-game thing because dynamicmesh isn't initialized
	if (GetWorld()->IsGameWorld()) { const FVector3d LocalClientPawnLocation = LocalClientPawn->GetActorLocation(); }
	else { const FVector3d LocalClientPawnLocation = FVector3d(0, 0, 0); }
	const int NumPointsPerLine = FMath::FloorToInt((RenderDistance * 2.0f) / Resolution) + 1;

	TArray<int32> Verticies;
	int64 ReserveCount = (int64)NumPointsPerLine * (int64)NumPointsPerLine;
	if (ReserveCount > TNumericLimits<int32>::Max()) ReserveCount = TNumericLimits<int32>::Max();
	Verticies.Reserve((int32)ReserveCount);
	GeneratedVertexLocations.Reserve((int32)ReserveCount);

	WorldGenRunnable->Run();

	// create vertices in row-major order: x changes fastest (this makes it faster?)
	for (int IndexY = 0; IndexY < NumPointsPerLine; ++IndexY) {
		float VertexY = -RenderDistance + IndexY * Resolution;

		for (int IndexX = 0; IndexX < NumPointsPerLine; ++IndexX) {
			float VertexX = -RenderDistance + IndexX * Resolution;

			// create vertex and remember its index
			int32 NewVertex = Mesh.AppendVertex(GeneratedVertexLocations[0]);
			UE_LOG(LogLandscape, Log, TEXT("Location: %s"), *GeneratedVertexLocations[0].ToString());
			GeneratedVertexLocations.RemoveAt(0);    // Remove the first index because it won't be needed + easy indexing. May be slow though
			check(Mesh.IsVertex(NewVertex));
			Verticies.Add(NewVertex);
		}
	}

	// Clear the array for it to be generated again
	GeneratedVertexLocations = TArray<FVector>();

	// Create the triangles
	for (int IndexY = 0; IndexY < NumPointsPerLine - 1; ++IndexY) {
		for (int IndexX = 0; IndexX < NumPointsPerLine - 1; ++IndexX) {
			// Apparently making them into individual variables instead of an array is better
			const int TopLeftVertex = Verticies[IndexX + IndexY * NumPointsPerLine];             // top left
			const int TopRightVertex = Verticies[(IndexX + 1) + IndexY * NumPointsPerLine];       // top right
			const int BottomLeftVertex = Verticies[IndexX + (IndexY + 1) * NumPointsPerLine];       // bottom left
			const int BottomRightVertex = Verticies[(IndexX + 1) + (IndexY + 1) * NumPointsPerLine]; // bottom right

			// First triangle (top-left, bottom-left, bottom-right)
			Mesh.AppendTriangle(TopLeftVertex, BottomLeftVertex, BottomRightVertex);

			// Second triangle (top-left, bottom-right, top-right)
			Mesh.AppendTriangle(TopLeftVertex, BottomRightVertex, TopRightVertex);
		}
	}
}

void AWorldLandscape::PostGeneration(UE::Geometry::FDynamicMesh3& Mesh)
{
	
}

void AWorldLandscape::GenerateVertexLocations()
{
	const FVector LocalClientPawnLocation = LocalClientPawn->GetActorLocation();
	const int NumPointsPerLine = FMath::FloorToInt((RenderDistance * 2.0f) / Resolution) + 1;

	// create vertices in row-major order: x changes fastest (this makes it faster?)
	for (int IndexY = 0; IndexY < NumPointsPerLine; ++IndexY) {
		float VertexY = -RenderDistance + IndexY * Resolution;

		for (int IndexX = 0; IndexX < NumPointsPerLine; ++IndexX) {
			float VertexX = -RenderDistance + IndexX * Resolution;

			// get noise for height
			float Height = Noise->GetNoise(VertexX + LocalClientPawnLocation.X / 50, VertexY + LocalClientPawnLocation.Y / 50) * HeightScale;

			// create vertex and remember its index
			FVector3d NewVertex = FVector3d(VertexX + LocalClientPawnLocation.X / 50, VertexY + LocalClientPawnLocation.Y / 50, Height);
			GeneratedVertexLocations.Add(NewVertex);
		}
	}
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
