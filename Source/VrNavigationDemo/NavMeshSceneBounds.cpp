// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshSceneBounds.h"

#include "EngineUtils.h"

NavMeshSceneBounds::NavMeshSceneBounds()
{
}

NavMeshSceneBounds::NavMeshSceneBounds(UWorld* NewWorld)
{
	this->World = NewWorld;

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

void NavMeshSceneBounds::GetFloorBounds(FVector BoundsDifference, float MinFloorHeight, float MaxFloorHeight)
{
	if(ActorsArray.Num() == 0)
		ActorsArray = GetAllSceneActors();

	float X;
	float Y;
	float Z;

	for (AActor* Actor : ActorsArray)
	{

		FVector Origin;
		FVector BoxExtent;
		Actor->GetActorBounds(false, Origin, BoxExtent);

		Origin = Origin - BoundsDifference;

		if ((Origin.Z < MaxFloorHeight && Origin.Z > MinFloorHeight)/* || (MaxFloorHeight < MinFloorHeight && Origin.Z > MinFloorHeight)*/)
		{
			UE_LOG(LogTemp, Error, TEXT("Prendo questo actor: %s"), *Actor->GetName());

			if (Origin.X < 0)
			{
				X = Origin.X - BoxExtent.X;
			}
			else
			{
				X = BoxExtent.X + Origin.X;
			}

			if (Origin.Y < 0)
			{
				Y = Origin.Y - BoxExtent.Y;
			}
			else
			{
				Y = Origin.Y + BoxExtent.Y;
			}

			if (Origin.Z < 0)
			{
				Z = Origin.Z - BoxExtent.Z;
			}
			else
			{
				Z = BoxExtent.Z + Origin.Z;
			}


			if (X > MaxX)
			{
				MaxX = X;
			}

			if (X < MinX)
			{
				MinX = X;
			}


			if (Y > MaxY)
			{
				MaxY = Y;
			}

			if (Y < MinY)
			{
				MinY = Y;
			}

			if (Z > MaxZ)
			{
				MaxZ = Z;
			}
			else if (Z < MinZ)
			{
				MinZ = Z;
			}
			else
			{
				MinZ = Origin.Z - BoxExtent.Z;
			}
			
		}

	}
}

FVector NavMeshSceneBounds::GetOptimalNavMeshPosition(int Floor)
{
	FVector NewNavMeshPosition = FVector(0.f, 0.f, 0.f);

	FloorsArrayOrigin = GetAllFloorActorsPosition();

	UE_LOG(LogTemp, Error, TEXT("Numero di piani: %i"), FloorsArrayOrigin.Num());

	if (Floor >= 0 && Floor < FloorsArrayOrigin.Num())
	{
		if (FloorsArrayOrigin.Num() > Floor + 1)
		{
			GetFloorBounds(FVector(0.f, 0.f, 0.f), FloorsArrayOrigin[Floor], FloorsArrayOrigin[Floor + 1]);

			UE_LOG(LogTemp, Error, TEXT("Piano: %i - Low: %f Max: %f"), Floor, FloorsArrayOrigin[Floor], FloorsArrayOrigin[Floor + 1]);

		}
		else
		{
			GetFloorBounds(FVector(0.f, 0.f, 0.f), FloorsArrayOrigin[Floor], -1.f);
			UE_LOG(LogTemp, Error, TEXT("Ultimo piano: %i - Low: %f Max: %f"), Floor, FloorsArrayOrigin[Floor], -1.f);
		}

		UE_LOG(LogTemp, Error, TEXT("MaxX: %f MaxY: %f MaxZ: %f"), MaxX, MaxY, MaxZ);
		UE_LOG(LogTemp, Error, TEXT("MinX: %f MinY: %f MinZ: %f"), MinX, MinY, MinZ);
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


	ResetBounds();

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
			UE_LOG(LogTemp, Error, TEXT("Ultimo piano: %i - Low: %f Max: %f"), Floor, FloorsArrayOrigin[Floor]);
		}

		UE_LOG(LogTemp, Error, TEXT("MaxX: %f MaxY: %f MaxZ: %f"), MaxX, MaxY, MaxZ);
		UE_LOG(LogTemp, Error, TEXT("MinX: %f MinY: %f MinZ: %f"), MinX, MinY, MinZ);

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

void NavMeshSceneBounds::ResetBounds()
{
	MaxX = 0;
	MaxY = 0;
	MaxZ = 0;

	MinX = 0;
	MinY = 0;
	MinZ = 0;
}
