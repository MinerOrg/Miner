// Copyright Schuyler Zheng. All Rights Reserved.

#include "WorldGenerationRunnable.h"
#include "WorldLandscape.h"

FWorldGenerationRunnable::FWorldGenerationRunnable(AWorldLandscape* WorldLandscape, UWorld* World)
{
	OwnerLandscape = WorldLandscape;
	CurrentWorld = World;
}

FWorldGenerationRunnable::~FWorldGenerationRunnable()
{

}

bool FWorldGenerationRunnable::Init()
{


	return true;
}

uint32 FWorldGenerationRunnable::Run()
{
	check(IsValid(CurrentWorld));
	if (CurrentWorld->IsGameWorld()) OwnerLandscape->GenerateVertexLocations();

	return 0;
}

void FWorldGenerationRunnable::Stop()
{

}

void FWorldGenerationRunnable::Exit()
{

}

FSingleThreadRunnable* FWorldGenerationRunnable::GetSingleThreadInterface()
{
	return nullptr;
}
