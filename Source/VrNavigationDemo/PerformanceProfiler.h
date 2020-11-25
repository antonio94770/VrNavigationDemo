// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	void GetNavigationProfilingData(FString Stat, TArray<FString>*, EComplexStatField::Type DataType);

private:

	TArray<FString> IncAVGArray_RecastBuildCompressedLayers;
	TArray<FString> IncMAXArray_RecastBuildCompressedLayers;

	TArray<FString> IncAVGArray_RecastBuildLayers;
	TArray<FString> IncMAXArray_RecastBuildLayers;

	TArray<FString> IncAVGArray_Navigation_BuildTime;
	TArray<FString> IncMAXArray_Navigation_BuildTime;

	TArray<FString> IncAVGArray_Navigation_GenerateNavigationDataLayer;
	TArray<FString> IncMAXArray_Navigation_GenerateNavigationDataLayer;

	TArray<FString> IncAVGArray_Navigation_RecastMemory;
	TArray<FString> IncMAXArray_Navigation_RecastMemory;

	TArray<FString> IncAVGArray_RecastBuildRegions;
	TArray<FString> IncMAXArray_RecastBuildRegions;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetAllProfilingData();
	void ResetTick();
	bool SaveArrayToFile(FString Filename, bool AllowOverWriting = true);

public:
	int32 MaxNumberOfTicks = 100;
	int32 CurrentTick = 100;

	bool bProfiling = false;

};
