// Copyright Schuyler Zheng. All Rights Reserved.

#include "WorldGenerationRunnable.h"
#include "WorldLandscape.h"

FWorldGenerationRunnable::FWorldGenerationRunnable(AWorldLandscape* WorldLandscape, int Seed, FVector3d PlayerLocation)
{
	OwnerLandscape = WorldLandscape;
	LandscapeSeed = Seed;
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
	OwnerLandscape->GenerateVertexLocations();

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
