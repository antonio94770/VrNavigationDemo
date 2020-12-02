// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshSceneBounds.h"
#include "NavMeshController.h"
#include "EngineUtils.h"

NavMeshSceneBounds::NavMeshSceneBounds()
{
}

NavMeshSceneBounds::NavMeshSceneBounds(UWorld* NewWorld)
{
	this->World = NewWorld;

	if (FloorsArrayOrigin.Num() == 0)
		FloorsArrayOrigin = GetAllFloorActorsPosition();

	ResetBounds();
}

NavMeshSceneBounds::NavMeshSceneBounds(UWorld* NewWorld, NavMeshController& object)
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


TArray<float> NavMeshSceneBounds::GetAllFloorActorsPosition()
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


TArray<AActor* > NavMeshSceneBounds::GetAllSceneActors()
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

void NavMeshSceneBounds::GetFloorBounds(FVector BoundsDifference, float MinHeight, float MaxHeight)
{
	if(ActorsArray.Num() == 0)
		ActorsArray = GetAllSceneActors();

	float ActorXMax, ActorXMin;
	float ActorYMax, ActorYMin;
	float ActorZMax, ActorZMin;


	/*float X;
	float Y;
	float Z;*/


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

		//UE_LOG(LogTemp, Error, TEXT("MESH TYPE: %f"), NavMeshControllerInstance->NavMeshType)


		if (bSingleMode ||
			((Origin.Z < MaxHeight - BoundsDifference.Z && Origin.Z > MinHeight - BoundsDifference.Z) || 
			(MaxHeight < MinHeight && Origin.Z > MinHeight - BoundsDifference.Z)
			))
		{
			//UE_LOG(LogTemp, Error, TEXT("Prendo questo actor: %s"), *Actor->GetName());


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
				//UE_LOG(LogTemp, Error, TEXT("ENTRA QUI"));
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

FVector NavMeshSceneBounds::GetOptimalNavMeshPosition(int Floor)
{
	FVector NewNavMeshPosition = FVector(0.f, 0.f, 0.f);

	if(FloorsArrayOrigin.Num() == 0)
		FloorsArrayOrigin = GetAllFloorActorsPosition();

	//UE_LOG(LogTemp, Error, TEXT("Numero di piani: %i"), FloorsArrayOrigin.Num());

	UE_LOG(LogTemp, Error, TEXT("Piano: %i"), Floor);

	if (Floor >= 0 && Floor < FloorsArrayOrigin.Num())
	{
		if (FloorsArrayOrigin.Num() > Floor + 1)
		{
			MinFloorHeight = FloorsArrayOrigin[Floor];
			MaxFloorHeight = FloorsArrayOrigin[Floor + 1];
			GetFloorBounds(FVector(0.f, 0.f, 0.f), MinFloorHeight, MaxFloorHeight);

			//UE_LOG(LogTemp, Error, TEXT("Piano: %i - Low: %f Max: %f"), Floor, FloorsArrayOrigin[Floor], FloorsArrayOrigin[Floor + 1]);

		}
		else
		{
			MinFloorHeight = FloorsArrayOrigin[Floor];
			MaxFloorHeight = -1.f;
			GetFloorBounds(FVector(0.f, 0.f, 0.f), FloorsArrayOrigin[Floor], -1.f);
			//UE_LOG(LogTemp, Error, TEXT("Ultimo piano: %i - Low: %f Max: %f"), Floor, FloorsArrayOrigin[Floor], -1.f);
		}

		//UE_LOG(LogTemp, Warning, TEXT("MaxX: %f MaxY: %f MaxZ: %f"), MaxX, MaxY, MaxZ);
		//UE_LOG(LogTemp, Warning, TEXT("MinX: %f MinY: %f MinZ: %f"), MinX, MinY, MinZ);
		float NewX = (MaxX + MinX) / 2;
		float NewY = (MaxY + MinY) / 2;
		float NewZ = (MaxZ + MinZ) / 2;

		NewNavMeshPosition = FVector(NewX, NewY, NewZ);
	}

	return NewNavMeshPosition;
}


FVector NavMeshSceneBounds::GetNavMeshBounds(FVector NavMeshPosition, int Floor)
{
	float FinalX;
	float FinalY;
	float FinalZ;


	//ResetBounds();

	if (Floor >= 0 && Floor < FloorsArrayOrigin.Num())
	{
		if (FloorsArrayOrigin.Num() > Floor + 1)
		{
			GetFloorBounds(NavMeshPosition, FloorsArrayOrigin[Floor], FloorsArrayOrigin[Floor + 1]);
			UE_LOG(LogTemp, Error, TEXT("Piano: %i"), Floor);
		}
		else
		{
			GetFloorBounds(NavMeshPosition, FloorsArrayOrigin[Floor], -1.f);
			//UE_LOG(LogTemp, Error, TEXT("Ultimo piano: %i - Low: %f Max: %f"), Floor, FloorsArrayOrigin[Floor]);
		}

		//UE_LOG(LogTemp, Warning, TEXT("MaxX: %f MaxY: %f MaxZ: %f"), MaxX, MaxY, MaxZ);
		//UE_LOG(LogTemp, Warning, TEXT("MinX: %f MinY: %f MinZ: %f"), MinX, MinY, MinZ);

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

int NavMeshSceneBounds::GetNumberOfFloors()
{
	return FloorsArrayOrigin.Num();
}

float NavMeshSceneBounds::GetMinFloorHeight()
{
	return MinFloorHeight;
}

float NavMeshSceneBounds::GetMaxFloorHeight()
{
	return MaxFloorHeight;
}

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
