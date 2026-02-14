// Copyright Schuyler Zheng. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UDynamicMesh.h"
#include "Components/DynamicMeshComponent.h"
#include "FastNoiseLiteTypes.h"
#include "WorldLandscape.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLandscape, Log, All);

DECLARE_MULTICAST_DELEGATE(FTerrainDataGeneratedDelegate);

USTRUCT(BlueprintType)
struct FTerrainMaterials {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Grass Material"))
	UMaterialInterface* GrassMaterial;
};

USTRUCT(BlueprintType)
struct FWorldGenerationData {
	GENERATED_BODY();

public:
	UPROPERTY(EditAnywhere, meta = (ToolTip = "The number that controls all randomness in terrain"))
	int Seed = 1337;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "How much distance to go until checking the noise again."))
	double Resolution = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Height Scale of the Landscape"))
	double HeightScale = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Chunk spacing/Distance to go until make chunk follow"))
	double ChunkDistance = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "How far the chunk should go"))
	double RenderDistance = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "How big the value of the cellular noise has to be inorder to count as a plate edge.", ClampMin = 0, ClampMax = 1))
	double PlateBoarderThreshhold = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "How big the value of the cellular noise has to be inorder to count as a plate edge."))
	int PlateBoarderCheckAttempts = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "Landscape Materials"))
	FTerrainMaterials LandscapeMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ToolTip = "The amount to multiply the value of plate tectonics by."))
	double PlateTectonicsHeightScale = 100;
};

/**
 * AWorldLandscape is an Actor that generates a dynamic landscape mesh based on a seed
 */

class FastNoiseLite;
class FWorldGenerationRunnable;

UCLASS(ConversionRoot, ComponentWrapperClass, ClassGroup = DynamicMesh, meta = (ChildCanTick), MinimalAPI)
class AWorldLandscape : public AActor
{
	GENERATED_BODY()

public:
	AWorldLandscape();

	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	UDynamicMeshComponent* GetDynamicMeshComponent() const { return DynamicMeshComponent; }

	//
	// Mesh Pool support. Meshes can be locally allocated from the Mesh Pool
	// in Blueprints, and then released back to the Pool and re-used. This
	// avoids generating temporary UDynamicMesh instances that need to be
	// garbage-collected. See UDynamicMeshPool for more details.
	//

	/** Control whether the DynamicMeshPool will be created when requested via GetComputeMeshPool() */
	UPROPERTY(Category = "DynamicMeshActor|Advanced", EditAnywhere, BlueprintReadWrite)
	bool bEnableComputeMeshPool = true;

	/** Access the compute mesh pool */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	UDynamicMeshPool* GetComputeMeshPool();

	/** Request a compute mesh from the Pool, which will return a previously-allocated mesh or add and return a new one. If the Pool is disabled, a new UDynamicMesh will be allocated and returned. */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	UDynamicMesh* AllocateComputeMesh();

	/** Release a compute mesh back to the Pool */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	bool ReleaseComputeMesh(UDynamicMesh* Mesh);

	/** Release all compute meshes that the Pool has allocated */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	void ReleaseAllComputeMeshes();

	/** Release all compute meshes that the Pool has allocated, and then release them from the Pool, so that they will be garbage-collected */
	UFUNCTION(BlueprintCallable, Category = DynamicMeshActor)
	void FreeAllComputeMeshes();

	// I had to make these variables public, so that landscape data could be generated
	// in the worldgenerationrunnable

	UPROPERTY(Transient)
	TObjectPtr<UDynamicMesh> DynamicMesh;

	FastNoiseLite BasicLandNoise;

	FastNoiseLite PlateTectonicsNoise;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Landscape")
	FWorldGenerationData LandscapeData;

	UPROPERTY(BlueprintReadOnly, meta = (Tooltip = "The local pawn for this client. Does not need to be changed by blueprints because it is automatically set at beginplay in c++."))
	APawn* LocalClientPawn = nullptr;

	FTerrainDataGeneratedDelegate ApplyTerrainDataDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetupNoise();
	void GenerateTerrain();

	void SetNoiseParameters(FastNoiseLite& NoiseObject, const FNoiseSettings& NoiseSettings);

	UPROPERTY(Category = DynamicMeshActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh", AllowPrivateAccess = "true"))
	TObjectPtr<class UDynamicMeshComponent> DynamicMeshComponent;

	/** The internal Mesh Pool, for use in DynamicMeshActor BPs. Use GetComputeMeshPool() to access this, as it will only be created on-demand if bEnableComputeMeshPool = true */
	UPROPERTY(Transient)
	TObjectPtr<UDynamicMeshPool> DynamicMeshPool;

	UPROPERTY(EditAnywhere, Category = "Noise")
	FNoiseSettings BasicLandNoiseSettings {
		0.1f,											// Frequency
		NoiseType_Perlin, 								// Noise Type
		RotationType3D_None,							// Rotation Type 3D
		FractalType_FBm,								// Fractal Type
		3,												// Fractal Octaves
		2.0f,											// Fractal Lacunarity
		0.5f,											// Fractal Gain
		0.0f,											// Fractal Weighted Strength
		2.0f,											// Fractal Ping Pong Strength
		CellularDistanceFunction_EuclideanSq,			// Cellular Distance Function
		CellularReturnType_Distance,					// Cellular Return Type
		1.0f,											// Cellular Jitter
		DomainWarpType_OpenSimplex2,					// Domain Warp Type
		-4.5f											// Domain Warp Amp
	};

	UPROPERTY(EditAnywhere, Category = "Noise")
	FNoiseSettings PlateTectonicsNoiseSettings{
		0.01f,											// Frequency
		NoiseType_Cellular, 								// Noise Type
		RotationType3D_None,							// Rotation Type 3D
		FractalType_FBm,								// Fractal Type
		3,												// Fractal Octaves
		2.0f,											// Fractal Lacunarity
		0.5f,											// Fractal Gain
		0.0f,											// Fractal Weighted Strength
		2.0f,											// Fractal Ping Pong Strength
		CellularDistanceFunction_EuclideanSq,			// Cellular Distance Function
		CellularReturnType_Distance,					// Cellular Return Type
		1.0f,											// Cellular Jitter
		DomainWarpType_OpenSimplex2,					// Domain Warp Type
		-4.5f											// Domain Warp Amp
	};

	UE::Geometry::EValidityCheckFailMode ValidityCheckFailMode = UE::Geometry::EValidityCheckFailMode::Ensure;

	FDynamicMesh3::FValidityOptions ValidityOptions = { false, false };

	FWorldGenerationRunnable* WorldGenerationRunnable = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Landscape Generation")
	TArray<FVector> GeneratedVertexLocations;

	UPROPERTY(BlueprintReadWrite)
	FVector LastPlayerLocation = FVector::ZeroVector;
};
