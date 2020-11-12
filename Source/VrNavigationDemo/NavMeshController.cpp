// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshController.h"

#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavMesh/RecastNavMesh.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include <VrNavigationDemo\OptimizeNavMeshScene.h>
#include <VrNavigationDemo\NavMeshSceneBounds.h>


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
		//OptimizeScene();
		//SetupNavMeshSettings();
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


void NavMeshController::RefreshNavMeshBounds()
{
	if (World != nullptr && navigationSystemV1 != nullptr)
	{
		NavMeshSceneBounds SceneBounds = NavMeshSceneBounds(World);
		FVector NavBox = SceneBounds.GetNavMeshBounds();

		for (TActorIterator<ANavMeshBoundsVolume> ActorItr(World); ActorItr; ++ActorItr) {
			arrayOfNavMeshBoundsVolume.Add(*ActorItr);
		}

		for (ANavMeshBoundsVolume* Volume : arrayOfNavMeshBoundsVolume)
		{
			Volume->GetRootComponent()->SetMobility(EComponentMobility::Stationary);

			Volume->SetActorRelativeScale3D(NavBox);
			Volume->GetRootComponent()->UpdateBounds();


			Volume->GetRootComponent()->SetMobility(EComponentMobility::Static);

			//Volume->GetBrushBuilder()


			navigationSystemV1->OnNavigationBoundsUpdated(Volume);


			//UE_LOG(LogTemp, Error, TEXT("Number of NavMeshVolume: %i"), arrayOfNavMeshBoundsVolume.Num());
		}
	}
}


void NavMeshController::SpawnNavMesh()
{
	//Non funziona lo spawn del navmesh a runtime
	if (World != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SPAWN"));

		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		//ANavMeshBoundsVolume* Volume = World->SpawnActor<ANavMeshBoundsVolume>(NavMeshBoundsVolume, FVector(0.f, 0.f, 0.f), Rotation, SpawnInfo);

		ANavMeshBoundsVolume* Volume = NewObject<ANavMeshBoundsVolume>(ANavMeshBoundsVolume::StaticClass(), TEXT("NavMeshVolume"));

		Volume->GetRootComponent()->SetMobility(EComponentMobility::Movable);
		Volume->GetRootComponent()->SetWorldLocation(FVector(0.f, 0.f, 0.f));

		Volume->GetRootComponent()->RegisterComponent();
		navigationSystemV1->OnNavigationBoundsUpdated(Volume);
		//World->SpawnActor<ARecastNavMesh>(RecastNavmesh, FVector(0.f, 0.f, 0.f), Rotation, SpawnInfo);
	}
}
