// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshSceneBounds.h"
#include "NavMeshController.h"
#include "EngineUtils.h"

NavMeshSceneBounds::NavMeshSceneBounds()
{
}

//We get all
NavMeshSceneBounds::NavMeshSceneBounds(UWorld* NewWorld, const NavMeshController& object)
{
	this->World = NewWorld;

	if (FloorsArrayOrigin.Num() == 0)
		FloorsArrayOrigin = GetAllFloorActorsPosition();
	

	if (object.NavMeshMode == ENavMeshTypeController::SINGLEMODE)
	{
		bSingleMode = true;
	}

	ResetBounds();
}

NavMeshSceneBounds::~NavMeshSceneBounds()
{

}

//Function to edit in DC: for every floor actor we get their position
TArray<float> NavMeshSceneBounds::GetAllFloorActorsPosition() const
{
	TArray<float > FloorPositions;

	if (World != nullptr)
	{
		for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
		{
			if (ActorItr->Tags.Contains("Floor"))
			{

				FVector Origin;
				FVector BoxExtent;
				ActorItr->GetActorBounds(false, Origin, BoxExtent);

				if(!FloorPositions.Contains(Origin.Z))
					FloorPositions.Add(Origin.Z);

			}
		}
	}
	return FloorPositions;
}


//Function to edit in DC: we get all the actors of the scene with a particular tag
TArray<AActor* > NavMeshSceneBounds::GetAllSceneActors() const
{
	TArray<AActor*> ActorsList;
	
	if (World != nullptr)
	{		
		for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
		{
			if (ActorItr->Tags.Contains("Wall") || ActorItr->Tags.Contains("Pillar") || ActorItr->Tags.Contains("Object"))
			{
				ActorsList.Add(*ActorItr);
			}
		}	
	}

	return ActorsList;
}


//Given Min X and Max X, it edit every min and max variables that all give the floor bounds
void NavMeshSceneBounds::GetFloorBounds(FVector BoundsDifference, float MinHeight, float MaxHeight)
{
	if(ActorsArray.Num() == 0)
		ActorsArray = GetAllSceneActors();

	float ActorXMax, ActorXMin;
	float ActorYMax, ActorYMin;
	float ActorZMax, ActorZMin;

	bool firstObject = false;

	ResetBounds();

	for (AActor* Actor : ActorsArray)
	{

		FVector Origin;
		FVector BoxExtent;
		Actor->GetActorBounds(false, Origin, BoxExtent);

		Origin.X = Origin.X - BoundsDifference.X;
		Origin.Y = Origin.Y - BoundsDifference.Y;
		Origin.Z = Origin.Z - BoundsDifference.Z;


		if (bSingleMode ||
			((Origin.Z < MaxHeight - BoundsDifference.Z && Origin.Z > MinHeight - BoundsDifference.Z) || 
			(MaxHeight < MinHeight && Origin.Z > MinHeight - BoundsDifference.Z)
			))
		{

			ActorXMin = Origin.X - BoxExtent.X;
			ActorXMax = Origin.X + BoxExtent.X;

			ActorYMin = Origin.Y - BoxExtent.Y;
			ActorYMax = Origin.Y + BoxExtent.Y;

			ActorZMin = Origin.Z - BoxExtent.Z;
			ActorZMax = Origin.Z + BoxExtent.Z;

			if (!firstObject)
			{
				MaxX = ActorXMax;
				MinX = ActorXMin;
				MaxY = ActorYMax;
				MinY = ActorYMin;
				MaxZ = ActorZMax;
				MinZ = ActorZMin;
				firstObject = true;
			}

			else
			{
				if (ActorXMax > MaxX)
				{
					MaxX = ActorXMax;
				}

				if (ActorYMax > MaxY)
				{
					MaxY = ActorYMax;
				}

				if (ActorZMax > MaxZ)
				{
					MaxZ = ActorZMax;
				}


				if (ActorXMin < MinX)
				{
					MinX = ActorXMin;
				}

				if (ActorYMin < MinY)
				{
					MinY = ActorYMin;
				}

				if (ActorZMin < MinZ)
				{
					MinZ = ActorZMin;
				}
			}
		
		}
	}
}


//Return navmesh center position for the current floor
FVector NavMeshSceneBounds::GetOptimalNavMeshPosition(int Floor)
{
	FVector NewNavMeshPosition = FVector(0.f, 0.f, 0.f);

	if(FloorsArrayOrigin.Num() == 0)
		FloorsArrayOrigin = GetAllFloorActorsPosition();


	UE_LOG(LogTemp, Error, TEXT("Piano: %i"), Floor);

	if (Floor >= 0 && Floor < FloorsArrayOrigin.Num())
	{
		if (FloorsArrayOrigin.Num() > Floor + 1)
		{
			MinFloorHeight = FloorsArrayOrigin[Floor];
			MaxFloorHeight = FloorsArrayOrigin[Floor + 1];
			GetFloorBounds(FVector(0.f, 0.f, 0.f), MinFloorHeight, MaxFloorHeight);
		}
		else
		{
			MinFloorHeight = FloorsArrayOrigin[Floor];
			MaxFloorHeight = -1.f;
			GetFloorBounds(FVector(0.f, 0.f, 0.f), FloorsArrayOrigin[Floor], -1.f);
		}

		float NewX = (MaxX + MinX) / 2;
		float NewY = (MaxY + MinY) / 2;
		float NewZ = (MaxZ + MinZ) / 2;

		NewNavMeshPosition = FVector(NewX, NewY, NewZ);
	}

	return NewNavMeshPosition;
}


//Return navmesh bounds from the current floor and the current navmesh position
FVector NavMeshSceneBounds::GetNavMeshBounds(FVector NavMeshPosition, int Floor)
{
	float FinalX;
	float FinalY;
	float FinalZ;


	if (Floor >= 0 && Floor < FloorsArrayOrigin.Num())
	{
		if (FloorsArrayOrigin.Num() > Floor + 1)
		{
			GetFloorBounds(NavMeshPosition, FloorsArrayOrigin[Floor], FloorsArrayOrigin[Floor + 1]);
		}
		else
		{
			GetFloorBounds(NavMeshPosition, FloorsArrayOrigin[Floor], -1.f);
		}

		if (fabs(MinX) < MaxX)
			FinalX = MaxX * 2;
		else
			FinalX = fabs(MinX) * 2;

		if (fabs(MinY) < MaxY)
			FinalY = MaxY * 2;
		else
			FinalY = fabs(MinY) * 2;

		if (fabs(MinZ) < MaxZ)
			FinalZ = MaxZ * 2;
		else
			FinalZ = fabs(MinZ) * 2;


		NavMeshBounds = FVector(FinalX, FinalY, FinalZ);
		UE_LOG(LogTemp, Error, TEXT("BOUNDS: %s"), *NavMeshBounds.ToString());
	}

	return NavMeshBounds;
}

//Return number of floors for the current scene
int NavMeshSceneBounds::GetNumberOfFloors() const
{
	return FloorsArrayOrigin.Num();
}


//Return the start height of the floor
float NavMeshSceneBounds::GetMinFloorHeight() const
{
	return MinFloorHeight;
}


//Return the end height of the floor
float NavMeshSceneBounds::GetMaxFloorHeight() const
{
	return MaxFloorHeight;
}


//Reset the bounds for the new calculation
void NavMeshSceneBounds::ResetBounds()
{
	MaxX = 0;
	MaxY = 0;
	MaxZ = 0;

	MinX = 0;
	MinY = 0;
	MinZ = 0;

	/*MinFloorHeight = 0;
	MaxFloorHeight = 0;*/
}
