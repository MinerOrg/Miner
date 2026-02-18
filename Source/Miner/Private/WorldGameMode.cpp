// Copyright Schuyler Zheng. All Rights Reserved.

#include "WorldGameMode.h"
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"

AWorldGameMode::AWorldGameMode()
{

}

void AWorldGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Tmp: Just set all players to spawn in the same location
	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		APlayerCharacter* Player = *It;

		SetPlayerSpawn(Player);
	}
}

void AWorldGameMode::SetPlayerSpawn(AActor* Player)
{
	check(IsValid(Player));

	Player->SetActorLocation(FindPlayerSpawnLocation(), false, nullptr, ETeleportType::TeleportPhysics);
}

FVector AWorldGameMode::FindPlayerSpawnLocation() const
{
	FVector FinalSpawnLocation = FVector(0, 0, -SpawnCheckRaycastDistance);

	// Settings for the line trace
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	bool Finished = false;
	
	// Could use multilinetrace, but this is easier
	if (GetWorld()->LineTraceSingleByChannel(Hit, FinalSpawnLocation, FVector(0, 0, SpawnCheckRaycastDistance), LandscapeChannel, Params)) {
		// This means that the spawn location is above 
		return FVector(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z + 100);
	}
	else if (GetWorld()->LineTraceSingleByChannel(Hit, FVector(0, 0, SpawnCheckRaycastDistance), FinalSpawnLocation, LandscapeChannel, Params)) {
		// This means that the spawn location is below
		return FVector(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z + 100);
	}

	ensure(!"Couldn't find a landscape to put the player on, so giving world origin.");
	return FVector::ZeroVector;
}
