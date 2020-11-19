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

		for (TActorIterator<ANavMeshBoundsVolume> ActorItr(World); ActorItr; ++ActorItr) {
			arrayOfNavMeshBoundsVolume.AddUnique(*ActorItr);
		}
		SceneBounds = NavMeshSceneBounds(World);
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
		FVector NewMeshPosition;
		FVector NewNavMeshScale;

		for (ANavMeshBoundsVolume* Volume : arrayOfNavMeshBoundsVolume)
		{
			//UE_LOG(LogTemp, Error, TEXT("CHIAMO L'AGGIORNAMENTO"));

			Volume->GetRootComponent()->SetMobility(EComponentMobility::Movable);

			FVector Origin;
			FVector BoxExtent;
			Volume->GetActorBounds(false, Origin, BoxExtent);

			
			NewMeshPosition = SceneBounds.GetOptimalNavMeshPosition(CurrentFloor);
			Volume->SetActorLocation(NewMeshPosition);

			BoxExtent = BoxExtent * 2;
			NewNavMeshScale = SceneBounds.GetNavMeshBounds(Volume->GetActorLocation(), CurrentFloor);

			NewNavMeshScale = FVector(NewNavMeshScale.X / BoxExtent.X, NewNavMeshScale.Y / BoxExtent.Y, NewNavMeshScale.Z / BoxExtent.Z);
			
			Volume->SetActorRelativeScale3D(NewNavMeshScale);
			Volume->GetRootComponent()->UpdateBounds();


			Volume->GetRootComponent()->SetMobility(EComponentMobility::Static);


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

void NavMeshController::ChangeCurrentFloor(int NewFloor)
{
	this->CurrentFloor = NewFloor;
}
