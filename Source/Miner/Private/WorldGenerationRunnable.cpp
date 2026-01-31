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
	LastRenderDistance = -1.0;
	LocalClientPawnLocation = FVector3d::ZeroVector;
	NumPointsPerLine = 0;
	DynamicMesh = OwnerLandscape->AllocateComputeMesh();

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
		if (bGenerate) { GenerateDynamicMesh(); }
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

void FWorldGenerationRunnable::GenerateDynamicMesh()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(GenerateDynamicMesh);

	check(IsValid(OwnerLandscape));

	DynamicMesh->InitializeMesh();

	Verticies.Empty();
	VertexHeights.Empty();

	// Local Client Pawn Location always changes
	LocalClientPawnLocation = (CurrentWorld->IsGameWorld()) ? OwnerLandscape->LocalClientPawn->GetActorLocation() : FVector3d::ZeroVector;

	// These things only change if render distance changes
	if (OwnerLandscape->LandscapeData.RenderDistance != LastRenderDistance) {
		NumPointsPerLine = FMath::FloorToInt((OwnerLandscape->LandscapeData.RenderDistance * 2.0f) / OwnerLandscape->LandscapeData.Resolution) + 1;
		int64 ReserveCount = NumPointsPerLine * NumPointsPerLine;    // You apparently can't use ^ for power, it is bitwise, and the Unreal power function requires floats
		if (NumPointsPerLine > TNumericLimits<int32>::Max()) ReserveCount = TNumericLimits<int32>::Max();
		Verticies.Reserve((int32)ReserveCount);
		VertexHeights.Reserve((int32)ReserveCount);
		LastRenderDistance = OwnerLandscape->LandscapeData.RenderDistance;
	}

	check(LastRenderDistance > 0);

	GenerateBasicHeights();
	ApplyPlateTectonics();
	FinalizeLandMesh();

	// signal the worker finished + make sure it doesn run again until requested
	bGenerate = false;
	// Move the completed local mesh to the UDynamicMesh on the game thread and then broadcast the delegate.
	TWeakObjectPtr<AWorldLandscape> WeakWorldLandscapeReference(OwnerLandscape);
	AsyncTask(ENamedThreads::GameThread, [WeakWorldLandscapeReference]() {
		if (WeakWorldLandscapeReference.IsValid() && IsValid(WeakWorldLandscapeReference->DynamicMesh))
		{
			// Broadcast that terrain data is ready (must be done on game thread?)
			WeakWorldLandscapeReference->ApplyTerrainDataDelegate.Broadcast();
		}
	});
}

void FWorldGenerationRunnable::GenerateBasicHeights()
{
	ModifyMesh([&](FVector CurrentVertexLocation) -> double {
		float Height = OwnerLandscape->BasicLandNoise.GetNoise(CurrentVertexLocation.X + LocalClientPawnLocation.X / 50, CurrentVertexLocation.Y + LocalClientPawnLocation.Y / 50) * OwnerLandscape->LandscapeData.HeightScale;

		return Height;
	});
}

void FWorldGenerationRunnable::ApplyPlateTectonics()
{
	ModifyMesh([&](FVector CurrentVertexLocation) -> double {


		return CurrentVertexLocation.Z;
	});
}

void FWorldGenerationRunnable::FinalizeLandMesh()
{
	int Index = 0;

	// Make the verticies
	for (int IndexY = 0; IndexY < NumPointsPerLine; ++IndexY) {
		float VertexY = -OwnerLandscape->LandscapeData.RenderDistance + IndexY * OwnerLandscape->LandscapeData.Resolution;

		for (int IndexX = 0; IndexX < NumPointsPerLine; ++IndexX) {
			float VertexX = -OwnerLandscape->LandscapeData.RenderDistance + IndexX * OwnerLandscape->LandscapeData.Resolution;

			// create vertex and remember its index (use local mesh)
			int32 NewVertex = (int32)DynamicMesh->GetMeshPtr()->AppendVertex(FVector(VertexX + LocalClientPawnLocation.X / 50, VertexY + LocalClientPawnLocation.Y / 50, VertexHeights[Index]));
			check(DynamicMesh->GetMeshPtr()->IsVertex(NewVertex));
			Verticies.Add(NewVertex);
			Index++;
		}
	}

	// Make the triangles
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
}

void FWorldGenerationRunnable::ModifyMesh(TFunctionRef<double(FVector)> ModifyFunc)
{
	int Index = 0;

	int64 ReserveCount = NumPointsPerLine * NumPointsPerLine;
	if (NumPointsPerLine > TNumericLimits<int32>::Max()) ReserveCount = TNumericLimits<int32>::Max();
	TArray<double> NewVertexHeights;
	NewVertexHeights.Reserve((int32)ReserveCount);

	for (int IndexY = 0; IndexY < NumPointsPerLine; ++IndexY) {
		float VertexY = -OwnerLandscape->LandscapeData.RenderDistance + IndexY * OwnerLandscape->LandscapeData.Resolution;

		for (int IndexX = 0; IndexX < NumPointsPerLine; ++IndexX) {
			float VertexX = -OwnerLandscape->LandscapeData.RenderDistance + IndexX * OwnerLandscape->LandscapeData.Resolution;

			const double CurrentVertexHeight = !VertexHeights.IsEmpty() ? VertexHeights[Index] : 0;
			const FVector CurrentVertexLocation = FVector(VertexX, VertexY, CurrentVertexHeight);    // Do all the calculations in local space, then transfer to world space later when finalizing the mesh

			double NewVertexHeight = ModifyFunc(CurrentVertexLocation);
			NewVertexHeights.Add(NewVertexHeight);
			Index++;
		}
	}
	
	VertexHeights = MoveTemp(NewVertexHeights);
}
