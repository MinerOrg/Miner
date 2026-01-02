// Copyright Schuyler Zheng. All Rights Reserved.

#include "WorldGenerationRunnable.h"
#include "WorldLandscape.h"

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
		if (bGenerate) { OwnerLandscape->GenerateVertexLocations(); }
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
