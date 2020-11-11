// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshController.h"

#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavMesh/RecastNavMesh.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include <VrNavigationDemo\OptimizeNavMeshScene.h>


NavMeshController::NavMeshController()
{
}

NavMeshController::~NavMeshController()
{
}

NavMeshController::NavMeshController(UWorld* NewWorld)
{

	this->World = NewWorld;

	if (World != nullptr)
	{
		this->navigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(World);

		//SpawnNavMesh();
		OptimizeScene();
		SetupNavMeshSettings();
	}
	else
		UE_LOG(LogTemp, Error, TEXT("No World Reference."));

}

void NavMeshController::OptimizeScene()
{
	if (World != nullptr)
	{
		/*OptimizeNavMeshScene Optimizer = OptimizeNavMeshScene(World);
		Optimizer.OptimizeAllMesh();*/
	}
}


void NavMeshController::SetupNavMeshSettings()
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


void NavMeshController::RefreshNavMesh()
{
	if (World != nullptr && navigationSystemV1 != nullptr)
	{

		for (TActorIterator<ANavMeshBoundsVolume> ActorItr(World); ActorItr; ++ActorItr) {
			arrayOfNavMeshBoundsVolume.Add(*ActorItr);
		}

		arrayOfNavMeshBoundsVolume[0]->SetActorRelativeScale3D(FVector(6000.f, 6000.f, 6000.f));
		arrayOfNavMeshBoundsVolume[0]->GetRootComponent()->UpdateBounds();

		navigationSystemV1->OnNavigationBoundsUpdated(arrayOfNavMeshBoundsVolume[0]);


		UE_LOG(LogTemp, Error, TEXT("Number of NavMeshVolume: %i"), arrayOfNavMeshBoundsVolume.Num());
	}
}


void NavMeshController::SpawnNavMesh()
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
