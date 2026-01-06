// Copyright Schuyler Zheng. All Rights Reserved.


#include "BaseDroppedItem.h"

// Sets default values
ABaseDroppedItem::ABaseDroppedItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true; // Enable replication for this actor
}

// Called when the game starts or when spawned
void ABaseDroppedItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseDroppedItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

