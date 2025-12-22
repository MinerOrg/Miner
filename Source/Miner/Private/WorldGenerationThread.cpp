// Copyright Schuyler Zheng. All Rights Reserved.

#include "WorldGenerationThread.h"
#include "WorldLandscape.h"

FWorldGenerationThread::FWorldGenerationThread(AWorldLandscape* WorldLandscape, int Seed, FVector3d PlayerLocation)
{
	OwnerLandscape = WorldLandscape;
	LandscapeSeed = Seed;
}

FWorldGenerationThread::~FWorldGenerationThread()
{

}

bool FWorldGenerationThread::Init()
{


	return true;
}

uint32 FWorldGenerationThread::Run()
{
	OwnerLandscape->GenerateVertexLocations();

	return 0;
}

void FWorldGenerationThread::Stop()
{

}

void FWorldGenerationThread::Exit()
{

}

FSingleThreadRunnable* FWorldGenerationThread::GetSingleThreadInterface()
{
	return nullptr;
}
