// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshManager.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavMesh/RecastNavMesh.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"

// Sets default values for this component's properties
UNavMeshManager::UNavMeshManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

UNavMeshManager::UNavMeshManager(UWorld* NewWorld)
{

	this->World = NewWorld;

	if (World != nullptr)
	{
		navigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(World);

		SpawnNavMesh();
		SetupNavMeshSettings();
	}
	else
		UE_LOG(LogTemp, Error, TEXT("No World Reference."));

}



// Called when the game starts
void UNavMeshManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UNavMeshManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UNavMeshManager::SetupNavMeshSettings()
{
	if (World != nullptr)
	{
		for (TActorIterator<ARecastNavMesh> ActorItr(World); ActorItr; ++ActorItr) {
			RecastNavmesh = *ActorItr;
		}

		if (RecastNavmesh != nullptr)
		{
			RecastNavmesh->TileSizeUU = 560.f;
			RecastNavmesh->CellSize = 20.f;
			RecastNavmesh->CellHeight = 15.f;
		}
	}
}


void UNavMeshManager::RefreshNavMesh()
{
	if (World != nullptr && navigationSystemV1 != nullptr)
	{

		for (TActorIterator<ANavMeshBoundsVolume> ActorItr(World); ActorItr; ++ActorItr) {
			arrayOfNavMeshBoundsVolume.Add(*ActorItr);
		}

		//arrayOfNavMeshBoundsVolume[0]->GetRootComponent()->SetMobility(EComponentMobility::Stationary);
		arrayOfNavMeshBoundsVolume[0]->SetActorRelativeScale3D(FVector(25000.f, 25000.f, 25000.f));
		arrayOfNavMeshBoundsVolume[0]->GetRootComponent()->UpdateBounds();

		//arrayOfNavMeshBoundsVolume[0]->GetRootComponent()->SetMobility(EComponentMobility::Static);


		navigationSystemV1->OnNavigationBoundsUpdated(arrayOfNavMeshBoundsVolume[0]);


		UE_LOG(LogTemp, Error, TEXT("Number of NavMeshVolume: %i"), arrayOfNavMeshBoundsVolume.Num());
	}
}


void UNavMeshManager::SpawnNavMesh()
{
	//Non funziona lo spawn del navmesh a runtime
	/*if (NavMeshBoundsVolume != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SPAWN"));

		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		GetWorld()->SpawnActor<ANavMeshBoundsVolume>(NavMeshBoundsVolume, FVector(0.f, 0.f, 0.f), Rotation, SpawnInfo);
		GetWorld()->SpawnActor<ARecastNavMesh>(RecastNavmesh, FVector(0.f, 0.f, 0.f), Rotation, SpawnInfo);
	}*/
}

