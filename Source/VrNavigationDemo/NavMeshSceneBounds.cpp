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

TArray<AActor* > NavMeshSceneBounds::GetAllSceneActor()
{

	TArray<AActor*> ActorsList;
	
	if (World != nullptr)
	{
		

		for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
		{
			if (ActorItr->Tags.Contains("Wall") || ActorItr->Tags.Contains("Pillar") || ActorItr->Tags.Contains("Object"))
			{
				/*auto n = ActorItr->GetComponentsBoundingBox();
				UE_LOG(LogTemp, Error, TEXT("Actor %s: %s"), ActorItr->GetName(), n.ToString());*/
				ActorsList.Add(*ActorItr);
			}
		}		
	}

	return ActorsList;
}

void NavMeshSceneBounds::GetSceneBounds(FVector BoundsDifference)
{
	ActorsArray = GetAllSceneActor();

	float X;
	float Y;
	float Z;

	for (AActor* Actor : ActorsArray)
	{

		FVector Origin;
		FVector BoxExtent;
		Actor->GetActorBounds(false, Origin, BoxExtent);

		FVector Add = Origin + BoxExtent;

		UE_LOG(LogTemp, Error, TEXT("Actor %s: %s"), *Actor->GetName(), *Add.ToString());

		Origin = Origin - BoundsDifference;

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

	}

	/*float FinalX;
	float FinalY;
	float FinalZ;

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


	NavMeshBounds = FVector(FinalX, FinalY, FinalZ);*/
	//NavMeshBounds = FVector(fabs(MinX), fabs(MinY), fabs(MinZ) );


	/*UE_LOG(LogTemp, Error, TEXT("MaxX: %f MaxY: %f MaxZ: %f"), FinalX, FinalY, FinalZ);
	UE_LOG(LogTemp, Error, TEXT("Bounds: %s"), *NavMeshBounds.ToString());


	return NavMeshBounds;*/
}

FVector NavMeshSceneBounds::GetOptimalNavMeshPosition()
{

	GetSceneBounds(FVector(0.f,0.f,0.f));

	UE_LOG(LogTemp, Error, TEXT("MaxX: %f MaxY: %f MaxZ: %f"), MaxX, MaxY, MaxZ);
	UE_LOG(LogTemp, Error, TEXT("MinX: %f MinY: %f MinZ: %f"), MinX, MinY, MinZ);

	float NewX = (MaxX - fabs(MinX))/2;
	float NewY = (MaxY - fabs(MinY))/2;
	float NewZ = (MaxZ - fabs(MinZ))/2;

	FVector NewNavMeshPosition = FVector(NewX, NewY, NewZ);

	return NewNavMeshPosition;
}


FVector NavMeshSceneBounds::GetNavMeshBounds(FVector NavMeshPosition)
{
	float FinalX;
	float FinalY;
	float FinalZ;

	ResetBounds();
	GetSceneBounds(NavMeshPosition);

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
