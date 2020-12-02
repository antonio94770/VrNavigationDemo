// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshController.h"

#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavMesh/RecastNavMesh.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include <VrNavigationDemo\NavMeshSceneBounds.h>
#include <VrNavigationDemo\PerformanceProfiler.h>


NavMeshController::NavMeshController():Optimizer(nullptr)
{
}


NavMeshController::~NavMeshController()
{
}


NavMeshController::NavMeshController(UWorld* NewWorld, ENavMeshTypeController NavMeshType, bool bOptimize):Optimizer(NewWorld)
{
	this->World = NewWorld;

	this->NavMeshMode = NavMeshType;


	if (World != nullptr)
	{
		this->NavigationSystemV1 = UNavigationSystemV1::GetNavigationSystem(World);

		SceneBounds = NavMeshSceneBounds(World, *this );
		FloorsNumber = SceneBounds.GetNumberOfFloors();

		UE_LOG(LogTemp, Error, TEXT("FLAG: %d"), NavMeshMode);

		int i = 0;
		for (TActorIterator<ANavMeshBoundsVolume> ActorItr(World); i < FloorsNumber && ActorItr; ++ActorItr, i++) {

			ArrayOfNavMeshBoundsVolume.AddUnique(*ActorItr);
			FVector Origin;
			FVector InitialBound;
			ActorItr->GetActorBounds(false, Origin, InitialBound);

			InitialBound = InitialBound * 2;
			InitialNavMeshBound.Add(InitialBound);


			if (NavMeshMode == ENavMeshTypeController::SINGLEMODE || NavMeshMode == ENavMeshTypeController::ONEFLOOR)
				break;
		}

		this->bOptimize = bOptimize;
	}
	else
		UE_LOG(LogTemp, Error, TEXT("No World Reference."));
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
	if (World != nullptr && NavigationSystemV1 != nullptr)
	{

		for (TActorIterator<APerformanceProfiler> ActorItr(World); ActorItr; ++ActorItr) {
			ActorItr->ResetTick();
		}

		

		for (ANavMeshBoundsVolume* Volume : ArrayOfNavMeshBoundsVolume)
		{

			FVector NewMeshPosition;
			FVector NewNavMeshScale;


			Volume->GetRootComponent()->SetMobility(EComponentMobility::Movable);
			
			NewMeshPosition = SceneBounds.GetOptimalNavMeshPosition(CurrentFloor);
			

			

			Volume->SetActorLocation(NewMeshPosition);

			NewNavMeshScale = SceneBounds.GetNavMeshBounds(Volume->GetActorLocation(), CurrentFloor);

			UE_LOG(LogTemp, Error, TEXT("ENTRO QUA: %s"), *NewMeshPosition.ToString());

			if(NavMeshMode == ENavMeshTypeController::MULTIPLEFLOOR)
				NewNavMeshScale = FVector(NewNavMeshScale.X / InitialNavMeshBound[CurrentFloor].X, NewNavMeshScale.Y / InitialNavMeshBound[CurrentFloor].Y, NewNavMeshScale.Z / InitialNavMeshBound[CurrentFloor].Z);
			else
				NewNavMeshScale = FVector(NewNavMeshScale.X / InitialNavMeshBound[0].X, NewNavMeshScale.Y / InitialNavMeshBound[0].Y, NewNavMeshScale.Z / InitialNavMeshBound[0].Z);

			UE_LOG(LogTemp, Error, TEXT("SIZE: %d"), InitialNavMeshBound.Num());
			Volume->SetActorRelativeScale3D(NewNavMeshScale);
			Volume->GetRootComponent()->UpdateBounds();
			Volume->GetRootComponent()->SetMobility(EComponentMobility::Static);

			//navigationSystemV1->CleanUp();

			if (bOptimize)
				Optimizer.ClearNavMesh();
			

			NavigationSystemV1->OnNavigationBoundsUpdated(Volume);
		

			if (bOptimize)
				Optimizer.OptimizeAllMesh(SceneBounds.GetMinFloorHeight(), SceneBounds.GetMaxFloorHeight());

			CurrentFloor++;
		}
	}
}


void NavMeshController::SpawnNavMesh()
{
	//Non funziona lo spawn del navmesh a runtime
	/*if (World != nullptr)
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
	}*/
}


void NavMeshController::ChangeCurrentFloor(int NewFloor)
{
	this->CurrentFloor = NewFloor;
}
