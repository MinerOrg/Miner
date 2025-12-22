// Copyright Schuyler Zheng. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSingleThreadRunnable;
class AWorldLandscape;

/**
 * The thread that handles world generation tasks
 * This will free up the game thread and allow for smoother gameplay
 */
class MINER_API FWorldGenerationThread : public FRunnable
{
public:
	FWorldGenerationThread(AWorldLandscape* WorldLandscape, int Seed, FVector3d PlayerLocation);
	~FWorldGenerationThread();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	virtual FSingleThreadRunnable* GetSingleThreadInterface() override;

protected:
	TObjectPtr<AWorldLandscape> OwnerLandscape;
	int LandscapeSeed;
};
