// Copyright Schuyler Zheng. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "UDynamicMesh.h"

class FSingleThreadRunnable;
class AWorldLandscape;

DECLARE_LOG_CATEGORY_EXTERN(LogWorldGenerationThread, Log, All);

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

	bool bGenerate = false;

	UPROPERTY(Transient)
	TObjectPtr<UDynamicMesh> DynamicMesh;

protected:
	void GenerateDynamicMesh();

	void GenerateBasicLand();
	void ApplyPlateTectonics();
	void FinalizeLandMesh();

	FRunnableThread* Thread;
	bool bRunThread = true;

	TObjectPtr<AWorldLandscape> OwnerLandscape;
	UWorld* CurrentWorld;

	TArray<int32> Verticies;
	TArray<FVector> VertexLocations;

	double LastRenderDistance;
	FVector3d LocalClientPawnLocation;
	int64 NumPointsPerLine;
};
