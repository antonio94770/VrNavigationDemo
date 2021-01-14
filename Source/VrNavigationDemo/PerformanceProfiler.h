// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Recast/Recast.h>
#include "Navmesh/RecastNavMesh.h"


#include "PerformanceProfiler.generated.h"

UCLASS()
class VRNAVIGATIONDEMO_API APerformanceProfiler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APerformanceProfiler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void GetNavigationProfilingData(const FString Stat, TArray<float>* const Array, const EComplexStatField::Type DataType);
	float GetMaxFromArray(const TArray<float>& Array, int StartPos, int EndPos) const;

private:

	/* Tarrays that stores all the stats values*/
	TArray<float> IncAVGArray_RecastBuildCompressedLayers;
	TArray<float> IncMAXArray_RecastBuildCompressedLayers;
	
	TArray<float> IncAVGArray_RecastBuildNavigation;
	TArray<float> IncMAXArray_RecastBuildNavigation;
	
	TArray<float> IncAVGArray_Navigation_TickAsyncBuild;
	TArray<float> IncMAXArray_Navigation_TickAsyncBuild;
	
	TArray<float> IncAVGArray_Navigation_TickMarkDirty;
	TArray<float> IncMAXArray_Navigation_TickMarkDirty;
	
	TArray<float> IncAVGArray_DebugNavOctree;
	TArray<float> IncMAXArray_DebugNavOctree;
	
	TArray<float> IncAVGArray_AddingActorsToNavOctree;
	TArray<float> IncMAXArray_AddingActorsToNavOctree;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetAllProfilingData();
	void ResetTick();
	bool SaveArrayToFile(FString Filename, bool AllowOverWriting = true) const;
	void DebugNavigationData(FString Filename) const;

public:

	//Used to count the intervall of our profiling operation
	int32 MaxNumberOfTicks = 100;

	//Set to 100 so we reset to zero when we want to start profiling
	int32 CurrentTick = 100;

};
