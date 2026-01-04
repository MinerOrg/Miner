// Copyright Schuyler Zheng. All Rights Reserved.

#include "WorldGenerationRunnable.h"
#include "WorldLandscape.h"
#include "FastNoiseLite.h"

DEFINE_LOG_CATEGORY(LogWorldGenerationThread);

#pragma region Main Thread Code
FWorldGenerationRunnable::FWorldGenerationRunnable(AWorldLandscape* WorldLandscape, UWorld* World)
{
	OwnerLandscape = WorldLandscape;
	CurrentWorld = World;

	Thread = FRunnableThread::Create(this, TEXT("World Generation Thread"));
}

FWorldGenerationRunnable::~FWorldGenerationRunnable()
{
	if (Thread) {
		Thread->Kill();
		delete Thread;
	}
}
#pragma endregion

bool FWorldGenerationRunnable::Init()
{
	UE_LOG(LogWorldGenerationThread, Log, TEXT("World Generation Thread Initialized."));

	return true;
}

uint32 FWorldGenerationRunnable::Run()
{
	check(IsValid(CurrentWorld));
	if (!CurrentWorld->IsGameWorld()) { return 1; }
	
	while (bRunThread) {
		if (bGenerate) { GenerateVertexLocations(); }
	}

	return 0;
}

void FWorldGenerationRunnable::Stop()
{
	bRunThread = false;
}

void FWorldGenerationRunnable::Exit()
{

}

FSingleThreadRunnable* FWorldGenerationRunnable::GetSingleThreadInterface()
{
	return nullptr;
}

void FWorldGenerationRunnable::GenerateVertexLocations()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(GenerateVertexLocations);

	check(IsValid(OwnerLandscape));
	check(IsValid(OwnerLandscape->DynamicMesh));

	OwnerLandscape->DynamicMesh->InitializeMesh();

	// Won't generate in a non-game thing because dynamicmesh isn't initialized, but it is here just incase I add it in the future
	const FVector3d LocalClientPawnLocation = (CurrentWorld->IsGameWorld()) ? OwnerLandscape->LocalClientPawn->GetActorLocation() : FVector3d::ZeroVector;
	const int NumPointsPerLine = FMath::FloorToInt((OwnerLandscape->LandscapeData.RenderDistance * 2.0f) / OwnerLandscape->LandscapeData.Resolution) + 1;

	TArray<int32> Verticies;
	int64 ReserveCount = (int64)NumPointsPerLine * (int64)NumPointsPerLine;
	if (ReserveCount > TNumericLimits<int32>::Max()) ReserveCount = TNumericLimits<int32>::Max();
	Verticies.Reserve((int32)ReserveCount);

	// create vertices in row-major order: x changes fastest (this makes it faster?)
	for (int IndexY = 0; IndexY < NumPointsPerLine; ++IndexY) {
		float VertexY = -OwnerLandscape->LandscapeData.RenderDistance + IndexY * OwnerLandscape->LandscapeData.Resolution;

		for (int IndexX = 0; IndexX < NumPointsPerLine; ++IndexX) {
			float VertexX = -OwnerLandscape->LandscapeData.RenderDistance + IndexX * OwnerLandscape->LandscapeData.Resolution;

			// get noise for height
			float Height = OwnerLandscape->Noise->GetNoise(VertexX + LocalClientPawnLocation.X / 50, VertexY + LocalClientPawnLocation.Y / 50) * OwnerLandscape->LandscapeData.HeightScale;

			// create vertex and remember its index
			int32 NewVertex = OwnerLandscape->DynamicMesh->GetMeshPtr()->AppendVertex(FVector3d(VertexX + LocalClientPawnLocation.X / 50, VertexY + LocalClientPawnLocation.Y / 50, Height));
			check(OwnerLandscape->DynamicMesh->GetMeshPtr()->IsVertex(NewVertex));
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
			OwnerLandscape->DynamicMesh->GetMeshPtr()->AppendTriangle(TopLeftVertex, BottomLeftVertex, BottomRightVertex);

			// Second triangle (top-left, bottom-right, top-right)
			OwnerLandscape->DynamicMesh->GetMeshPtr()->AppendTriangle(TopLeftVertex, BottomRightVertex, TopRightVertex);
		}
	}

	// signal the worker finished and schedule the mesh apply on the Game Thread
	bGenerate = false;

	// You can only broadcast delegates on the game thread, so convert it to the game thread 
	// Even though it worked on everytime except when you exit? IDK I asked github copilot. Maybe it was because the worldlandscape was being destroyed? 
	// The crash said it was because chaos physics was running on another thread that wasn't the game thread.
	TWeakObjectPtr<AWorldLandscape> WeakWorldLandscapeReference(OwnerLandscape);
	AsyncTask(ENamedThreads::GameThread, [WeakWorldLandscapeReference]() {
		if (WeakWorldLandscapeReference.IsValid())
		{
			WeakWorldLandscapeReference->ApplyTerrainDataDelegate.Broadcast();
		}
	});
}
