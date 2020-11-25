// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshController.h"

#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavMesh/RecastNavMesh.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include <VrNavigationDemo\OptimizeNavMeshScene.h>
#include <VrNavigationDemo\NavMeshSceneBounds.h>
#include <VrNavigationDemo\PerformanceProfiler.h>


NavMeshController::NavMeshController()
{
}


NavMeshController::~NavMeshController()
{
}

NavMeshController::NavMeshController(UWorld* NewWorld, bool bDifferentNavMeshForDifferentFloor)
{

	this->World = NewWorld;

	if (World != nullptr)
	{
		this->navigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(World);


		SceneBounds = NavMeshSceneBounds(World, *this);
		FloorsNumber = SceneBounds.GetNumberOfFloors();

		UE_LOG(LogTemp, Error, TEXT("FLAG: %d"), bDifferentNavMeshForDifferentFloor);

		int i = 0;
		for (TActorIterator<ANavMeshBoundsVolume> ActorItr(World); i < FloorsNumber && ActorItr; ++ActorItr, i++) {

			arrayOfNavMeshBoundsVolume.AddUnique(*ActorItr);
			FVector Origin;
			InitialNavMeshBound;
			ActorItr->GetActorBounds(false, Origin, InitialNavMeshBound);

			InitialNavMeshBound = InitialNavMeshBound * 2;

			if (!bDifferentNavMeshForDifferentFloor)
				break;
		}

		
		//SpawnNavMesh();
		//OptimizeScene();
		//SetupNavMeshSettings();
	}
	else
		UE_LOG(LogTemp, Error, TEXT("No World Reference."));

}


NavMeshController::NavMeshController(UWorld* NewWorld, ENavMeshTypeController NavMeshType)
{
	this->World = NewWorld;

	this->NavMeshMode = NavMeshType;

	if (World != nullptr)
	{
		this->navigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(World);


		SceneBounds = NavMeshSceneBounds(World, *this );
		FloorsNumber = SceneBounds.GetNumberOfFloors();

		UE_LOG(LogTemp, Error, TEXT("FLAG: %d"), NavMeshMode);

		int i = 0;
		for (TActorIterator<ANavMeshBoundsVolume> ActorItr(World); i < FloorsNumber && ActorItr; ++ActorItr, i++) {

			arrayOfNavMeshBoundsVolume.AddUnique(*ActorItr);
			FVector Origin;
			InitialNavMeshBound;
			ActorItr->GetActorBounds(false, Origin, InitialNavMeshBound);

			InitialNavMeshBound = InitialNavMeshBound * 2;

			if (NavMeshMode == ENavMeshTypeController::SINGLEMODE || NavMeshMode == ENavMeshTypeController::ONEFLOOR)
				break;
		}

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

		for (TActorIterator<APerformanceProfiler> ActorItr(World); ActorItr; ++ActorItr) {
			ActorItr->ResetTick();
		}

		FVector NewMeshPosition;
		FVector NewNavMeshScale;

		for (ANavMeshBoundsVolume* Volume : arrayOfNavMeshBoundsVolume)
		{
			

			Volume->GetRootComponent()->SetMobility(EComponentMobility::Movable);

			/*FVector Origin;
			FVector BoxExtent;
			Volume->GetActorBounds(false, Origin, BoxExtent);*/

			
			NewMeshPosition = SceneBounds.GetOptimalNavMeshPosition(CurrentFloor);
			Volume->SetActorLocation(NewMeshPosition);

			//BoxExtent = BoxExtent * 2;
			NewNavMeshScale = SceneBounds.GetNavMeshBounds(Volume->GetActorLocation(), CurrentFloor);

			UE_LOG(LogTemp, Error, TEXT("BoxExtent: %s"), *InitialNavMeshBound.ToString());

			NewNavMeshScale = FVector(NewNavMeshScale.X / InitialNavMeshBound.X, NewNavMeshScale.Y / InitialNavMeshBound.Y, NewNavMeshScale.Z / InitialNavMeshBound.Z);
			
			Volume->SetActorRelativeScale3D(NewNavMeshScale);
			Volume->GetRootComponent()->UpdateBounds();
			Volume->GetRootComponent()->SetMobility(EComponentMobility::Static);

			navigationSystemV1->OnNavigationBoundsUpdated(Volume);

			

			CurrentFloor++;
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
