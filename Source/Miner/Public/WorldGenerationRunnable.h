// Copyright Schuyler Zheng. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSingleThreadRunnable;
class AWorldLandscape;

/**
 * The thread that handles world generation tasks
 * This will free up the game thread and allow for smoother gameplay
 */
class MINER_API FWorldGenerationRunnable : public FRunnable
{
public:
	FWorldGenerationRunnable(AWorldLandscape* WorldLandscape, UWorld* World);
	~FWorldGenerationRunnable();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	virtual FSingleThreadRunnable* GetSingleThreadInterface() override;

protected:
	TObjectPtr<AWorldLandscape> OwnerLandscape;
	UWorld* CurrentWorld;
};
