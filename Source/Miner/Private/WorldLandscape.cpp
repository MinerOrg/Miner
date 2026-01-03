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

	DynamicMeshComponent->SetMaterial(0, DefaultLandscapeMaterial);

	SetRootComponent(DynamicMeshComponent);

	LastPlayerLocation = FVector::ZeroVector;
	LocalClientPawn = nullptr;
	WorldGenerationRunnable = nullptr;

	ApplyTerrainDataDelegate.AddUObject(this, &AWorldLandscape::GenerateTerrain);
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

	// Create the thread that generates the vertex locations
	WorldGenerationRunnable = new FWorldGenerationRunnable(this, GetWorld());

	// Pre-allocate vertex locations array
	const int NumPointsPerLine = FMath::FloorToInt((RenderDistance * 2.0f) / Resolution) + 1;
	int64 ReserveCount = (int64)NumPointsPerLine * (int64)NumPointsPerLine;
	if (ReserveCount > TNumericLimits<int32>::Max()) ReserveCount = TNumericLimits<int32>::Max();
	GeneratedVertexLocations.Reserve((int32)ReserveCount);

	SetupNoise();

	// Generate the mesh (last step)
	GenerateVertexLocations();
}

void AWorldLandscape::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	checkf(IsValid(LocalClientPawn), TEXT("Client Pawn bad"));
	FVector LocalClientPawnLocation = LocalClientPawn->GetActorLocation();

	// If the player has moved out of bounds, make the mesh follow them. (No Z check for now) (make sure that this runs last because if it is generating it will return)
	if (FMath::RoundToInt(LastPlayerLocation.X / ChunkDistance) * ChunkDistance != FMath::RoundToInt(LocalClientPawnLocation.X / ChunkDistance) * ChunkDistance || FMath::RoundToInt(LastPlayerLocation.Y / ChunkDistance) * ChunkDistance != FMath::RoundToInt(LocalClientPawnLocation.Y / ChunkDistance) * ChunkDistance /* || FMath::RoundToInt(LastPlayerLocation.Z / ChunkDistance) * ChunkDistance != LocalClientPawnLocation.Z */) {
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
	bCurrentlyGenerating = false;

	TRACE_CPUPROFILER_EVENT_SCOPE(GenerateTerrain);

	checkf(IsValid(DynamicMeshComponent), TEXT("Dynamic Mesh Component was bad"));
	checkf(IsValid(DynamicMesh), TEXT("Dynamic Mesh was bad"));
	checkf(Noise, TEXT("Noise was bad"));
	
	// EditMesh > just using notifymesh because more safe
	DynamicMesh->EditMesh([&](UE::Geometry::FDynamicMesh3& Mesh) {
		Mesh = *DynamicMesh->GetMeshPtr();

		// Final validity checks
		ensureMsgf(Mesh.CheckValidity(ValidityOptions, ValidityCheckFailMode), TEXT("Mesh was not valid"));
		ensureMsgf(Mesh.IsCompact(), TEXT("Mesh had gaps (was not compact)"));
	});
}

void AWorldLandscape::ApplyGeneratedMeshData(UE::Geometry::FDynamicMesh3& Mesh)
{
	
}

void AWorldLandscape::PostGeneration(UE::Geometry::FDynamicMesh3& Mesh)
{
	
}

void AWorldLandscape::GenerateVertexLocations()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(GenerateVertexLocations);

	DynamicMesh->InitializeMesh();

	// Won't generate in a non-game thing because dynamicmesh isn't initialized, but it is here just incase I add it in the future
	const FVector3d LocalClientPawnLocation = (GetWorld()->IsGameWorld()) ? LocalClientPawn->GetActorLocation() : FVector3d::ZeroVector;
	const int NumPointsPerLine = FMath::FloorToInt((RenderDistance * 2.0f) / Resolution) + 1;

	TArray<int32> Verticies;
	int64 ReserveCount = (int64)NumPointsPerLine * (int64)NumPointsPerLine;
	if (ReserveCount > TNumericLimits<int32>::Max()) ReserveCount = TNumericLimits<int32>::Max();
	Verticies.Reserve((int32)ReserveCount);

	// create vertices in row-major order: x changes fastest (this makes it faster?)
	for (int IndexY = 0; IndexY < NumPointsPerLine; ++IndexY) {
		float VertexY = -RenderDistance + IndexY * Resolution;

		for (int IndexX = 0; IndexX < NumPointsPerLine; ++IndexX) {
			float VertexX = -RenderDistance + IndexX * Resolution;

			// get noise for height
			float Height = Noise->GetNoise(VertexX + LocalClientPawnLocation.X / 50, VertexY + LocalClientPawnLocation.Y / 50) * HeightScale;

			// create vertex and remember its index
			int32 NewVertex = DynamicMesh->GetMeshPtr()->AppendVertex(FVector3d(VertexX + LocalClientPawnLocation.X / 50, VertexY + LocalClientPawnLocation.Y / 50, Height));
			check(DynamicMesh->GetMeshPtr()->IsVertex(NewVertex));
			Verticies.Add(NewVertex);
		}
	}

	// Create the triangles
	for (int IndexY = 0; IndexY < NumPointsPerLine - 1; ++IndexY) {
		for (int IndexX = 0; IndexX < NumPointsPerLine - 1; ++IndexX) {
			// Apparently making them into individual variables instead of an array is better
			const int TopLeftVertex = Verticies[IndexX + IndexY * NumPointsPerLine];             // top left
			const int TopRightVertex = Verticies[(IndexX + 1) + IndexY * NumPointsPerLine];       // top right
			const int BottomLeftVertex = Verticies[IndexX + (IndexY + 1) * NumPointsPerLine];       // bottom left
			const int BottomRightVertex = Verticies[(IndexX + 1) + (IndexY + 1) * NumPointsPerLine]; // bottom right

			// First triangle (top-left, bottom-left, bottom-right)
			DynamicMesh->GetMeshPtr()->AppendTriangle(TopLeftVertex, BottomLeftVertex, BottomRightVertex);

			// Second triangle (top-left, bottom-right, top-right)
			DynamicMesh->GetMeshPtr()->AppendTriangle(TopLeftVertex, BottomRightVertex, TopRightVertex);
		}
	}

	// signal the worker finished and schedule the mesh apply on the Game Thread
	WorldGenerationRunnable->bGenerate = false;

	// You can only broadcast delegates on the game thread, so convert it to the game thread 
	// Even though it worked on everytime except when you exit? IDK I asked github copilot. Maybe it was because the worldlandscape was being destroyed? 
	// The crash said it was because chaos physics was running on another thread that wasn't the game thread.
	TWeakObjectPtr<AWorldLandscape> WeakWorldLandscapeReference(this);
	AsyncTask(ENamedThreads::GameThread, [WeakWorldLandscapeReference]() {
		if (WeakWorldLandscapeReference.IsValid())
		{
			WeakWorldLandscapeReference->ApplyTerrainDataDelegate.Broadcast();
		}
	});
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
