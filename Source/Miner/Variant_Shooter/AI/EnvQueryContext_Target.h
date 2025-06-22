<<<<<<< HEAD
version https://git-lfs.github.com/spec/v1
oid sha256:968003be8df126859a32a73824a70418b48686e89947fe72ffa894c46d32e85b
size 563
=======
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Target.generated.h"

/**
 *  Custom EnvQuery Context that returns the actor currently targeted by an NPC
 */
UCLASS()
class MINER_API UEnvQueryContext_Target : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:

	/** Provides the context locations or actors for this EnvQuery */
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

};
>>>>>>> f652433 (fixed LFS in .h header files)
