// Fill out your copyright notice in the Description page of Project Settings.


#include "NavMeshSceneBounds.h"

#include "EngineUtils.h"

NavMeshSceneBounds::NavMeshSceneBounds()
{
}

NavMeshSceneBounds::NavMeshSceneBounds(UWorld* NewWorld)
{
	this->World = NewWorld;
}

NavMeshSceneBounds::~NavMeshSceneBounds()
{
}

TArray<AActor* > NavMeshSceneBounds::GetAllSceneActor()
{

	TArray<AActor*> ActorsList;
	
	if (World != nullptr)
	{
		

		for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
		{
			if (ActorItr->Tags.Contains("Wall") || ActorItr->Tags.Contains("Pillar") || ActorItr->Tags.Contains("Object") || ActorItr->Tags.Contains("Floor"))
			{
				/*auto n = ActorItr->GetComponentsBoundingBox();
				UE_LOG(LogTemp, Error, TEXT("Actor %s: %s"), ActorItr->GetName(), n.ToString());*/
				ActorsList.Add(*ActorItr);
			}
		}		
	}

	return ActorsList;
}

FVector NavMeshSceneBounds::GetNavMeshBounds()
{
	ActorsArray = GetAllSceneActor();

	float X;
	float Y;
	float Z;

	float MaxX = 0;
	float MaxY = 0;
	float MaxZ = 0;

	float MinX = 0;
	float MinY = 0;
	float MinZ = 0;

	for (AActor* Actor : ActorsArray)
	{
		/*auto n = FVector(
			Actor->GetComponentsBoundingBox().GetExtent().X,
			Actor->GetComponentsBoundingBox().GetExtent().Y,
			Actor->GetComponentsBoundingBox().GetExtent().Z
		);*/

		FVector Origin;
		FVector BoxExtent;
		Actor->GetActorBounds(false, Origin, BoxExtent);

		FVector Add = Origin + BoxExtent;

		//UE_LOG(LogTemp, Error, TEXT("Actor %s: %f"), *Actor->GetName(), Add.Y);

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
			Y = Origin.Y - BoxExtent.Y;
		}

		if (Origin.Z < 0)
		{
			Z =  Origin.Z - BoxExtent.Z;
		}
		else
		{
			Z = BoxExtent.Z + Origin.Z;
		}


		if (X > MaxX)
		{
			MaxX = X;
		}

		if (Y > MaxY)
		{
			MaxY = Y;
		}

		if (Z > MaxZ)
		{
			MaxZ = Z;
		}


		if (X < MinX)
		{
			MinX = X;
		}

		if (Y < MinY)
		{
			MinY = Y;
		}

		if (Z < MinZ)
		{
			MinZ = Z;
		}

		//UE_LOG(LogTemp, Error, TEXT("Actor %s: %s"), *Actor->GetName(), *Add.ToString());

	}

	NavMeshBounds = FVector(MaxX + fabs(MinX), MaxY + fabs(MinY), MaxZ + fabs(MinZ) );
	//NavMeshBounds = FVector(fabs(MinX), fabs(MinY), fabs(MinZ) );


	UE_LOG(LogTemp, Error, TEXT("Bounds: %s"), *NavMeshBounds.ToString());


	return NavMeshBounds;
}
