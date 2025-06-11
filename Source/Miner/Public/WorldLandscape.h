// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMeshActor.h"
#include "FastNoiseLite.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "WorldLandscape.generated.h"

/**
 * 
 */
UCLASS()
class MINER_API AWorldLandscape : public ADynamicMeshActor
{
	GENERATED_BODY()
	
	AWorldLandscape();

public:
	//TObjectPtr<FDynamicMesh3> Mesh;
	

	//TObjectPtr<FastNoiseLite> Noise;

protected:
	

private:
	
};
