// Fill out your copyright notice in the Description page of Project Settings.


#include "PerformanceProfiler.h"
#include "Stats\StatsData.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "NavigationSystem.h"
#include "AI/NavDataGenerator.h"
#include "NavMesh/RecastHelpers.h"
#include "AI/NavigationSystemHelpers.h"
#include "NavMesh/RecastNavMeshGenerator.h"


APerformanceProfiler::APerformanceProfiler()
{
	PrimaryActorTick.bCanEverTick = true;

}


void APerformanceProfiler::BeginPlay()
{
	Super::BeginPlay();	
}

// If the current tick is 0 the it starts profiling until CurrentTick reachs MaxNumberOfTicks
void APerformanceProfiler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTick < MaxNumberOfTicks)
	{
		GetAllProfilingData();
		CurrentTick++;
	}
}


// Given a stat name, and his type, we add the stat's value to his array
void APerformanceProfiler::GetNavigationProfilingData(const FString Stat, TArray<float>* const Array, const EComplexStatField::Type DataType)
{
	FGameThreadStatsData* ViewData = FLatestGameThreadStatsData::Get().Latest;
	if (ViewData != nullptr)
	{
		if (ViewData->NameToStatMap.FindRef(FName(Stat)))
		{
			int32 stat_data = int32(ViewData->NameToStatMap.FindRef(FName(Stat))->GetValue_int64(DataType));
			float final_value = float(stat_data) / 10000;

			Array->Add(final_value);
		}
		else
		{
			Array->Add(0.f);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Start STAT Navigation first!!"));
		Array->Add(0.f);
	}
}

// Here we define which stats we want to profile
void APerformanceProfiler::GetAllProfilingData()
{	
	GetNavigationProfilingData("STAT_Navigation_RecastBuildCompressedLayers", &IncAVGArray_RecastBuildCompressedLayers, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_RecastBuildCompressedLayers", &IncMAXArray_RecastBuildCompressedLayers, EComplexStatField::IncMax);
	GetNavigationProfilingData("STAT_Navigation_RecastBuildNavigation", &IncAVGArray_RecastBuildNavigation, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_RecastBuildNavigation", &IncMAXArray_RecastBuildNavigation, EComplexStatField::IncMax);
	GetNavigationProfilingData("STAT_Navigation_TickAsyncBuild", &IncAVGArray_Navigation_TickAsyncBuild, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_TickAsyncBuild", &IncMAXArray_Navigation_TickAsyncBuild, EComplexStatField::IncMax);
	GetNavigationProfilingData("STAT_Navigation_TickMarkDirty", &IncAVGArray_Navigation_TickMarkDirty, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_TickMarkDirty", &IncMAXArray_Navigation_TickMarkDirty, EComplexStatField::IncMax);
	GetNavigationProfilingData("STAT_DebugNavOctree", &IncAVGArray_DebugNavOctree, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_DebugNavOctree", &IncMAXArray_DebugNavOctree, EComplexStatField::IncMax);
	GetNavigationProfilingData("STAT_Navigation_AddingActorsToNavOctree", &IncAVGArray_AddingActorsToNavOctree, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_AddingActorsToNavOctree", &IncMAXArray_AddingActorsToNavOctree, EComplexStatField::IncMax);

}


// Function so save all the array's value to a file
bool APerformanceProfiler::SaveArrayToFile(FString Filename, bool AllowOverWriting /*= true*/) const
{
	FString SaveDirectory = FPaths::ProjectContentDir() + "\\Profiling\\" + Filename;
	UE_LOG(LogTemp, Error, TEXT("Path: %s"), *SaveDirectory);


	if (!AllowOverWriting)
	{
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*SaveDirectory))
		{
			return false;
		}
	}

	FString Text;

	Text = "IncAVGArray_RecastBuildCompressedLayers,\
			IncMAXArray_RecastBuildCompressedLayers,\
			IncAVGArray_RecastBuildNavigation,\
			IncMAXArray_RecastBuildNavigation,\
			IncAVGArray_Navigation_TickAsyncBuild,\
			IncMAXArray_Navigation_TickAsyncBuild,\
			IncAVGArray_Navigation_TickMarkDirty,\
			IncMAXArray_Navigation_TickMarkDirty,\
			IncAVGArray_DebugNavOctree,\
			IncMAXArray_DebugNavOctree,\
			IncAVGArray_AddingActorsToNavOctree, \
			IncMAXArray_AddingActorsToNavOctree";

	Text += LINE_TERMINATOR;

	if (IncAVGArray_RecastBuildCompressedLayers.Num() &&
		IncMAXArray_RecastBuildCompressedLayers.Num() &&
		IncAVGArray_RecastBuildNavigation.Num() &&
		IncMAXArray_RecastBuildNavigation.Num() &&
		IncAVGArray_Navigation_TickAsyncBuild.Num() &&
		IncMAXArray_Navigation_TickAsyncBuild.Num() &&
		IncAVGArray_Navigation_TickMarkDirty.Num() &&
		IncMAXArray_Navigation_TickMarkDirty.Num() &&
		IncAVGArray_DebugNavOctree.Num() &&
		IncMAXArray_DebugNavOctree.Num() &&
		IncAVGArray_AddingActorsToNavOctree.Num() &&
		IncMAXArray_AddingActorsToNavOctree.Num())
	{
		for (int i = 0; i < IncAVGArray_RecastBuildCompressedLayers.Num(); i++)
		{
			Text += FString::SanitizeFloat(IncAVGArray_RecastBuildCompressedLayers[i]) + "," +
				FString::SanitizeFloat(IncMAXArray_RecastBuildCompressedLayers[i]) + "," +
				FString::SanitizeFloat(IncAVGArray_RecastBuildNavigation[i]) + "," +
				FString::SanitizeFloat(IncMAXArray_RecastBuildNavigation[i]) + "," +
				FString::SanitizeFloat(IncAVGArray_Navigation_TickAsyncBuild[i]) + "," +
				FString::SanitizeFloat(IncMAXArray_Navigation_TickAsyncBuild[i]) + "," +
				FString::SanitizeFloat(IncAVGArray_Navigation_TickMarkDirty[i]) + "," +
				FString::SanitizeFloat(IncMAXArray_Navigation_TickMarkDirty[i]) + "," +
				FString::SanitizeFloat(IncAVGArray_DebugNavOctree[i]) + "," +
				FString::SanitizeFloat(IncMAXArray_DebugNavOctree[i]) + "," +
				FString::SanitizeFloat(IncAVGArray_AddingActorsToNavOctree[i]) + "," +
				FString::SanitizeFloat(IncMAXArray_AddingActorsToNavOctree[i]) +
				LINE_TERMINATOR;
		}

		float MaxAVG_RecastBuildCompressedLayers = 0.f;
		float MaxMAX_RecastBuildCompressedLayers = 0.f;
		float MaxAVG_RecastBuildNavigation = 0.f;
		float MaxMAX_RecastBuildNavigation = 0.f;
		float MaxAVG_Navigation_TickAsyncBuild = 0.f;
		float MaxMAX_Navigation_TickAsyncBuild = 0.f;
		float MaxAVG_Navigation_TickMarkDirty = 0.f;
		float MaxMAX_Navigation_TickMarkDirty = 0.f;
		float MaxAVG_DebugNavOctree = 0.f;
		float MaxMAX_DebugNavOctree = 0.f;
		float MaxAVG_AddingActorsToNavOctree = 0.f;
		float MaxMAX_AddingActorsToNavOctree = 0.f;


		for(int k = 0; k < IncAVGArray_RecastBuildCompressedLayers.Num(); k = k + MaxNumberOfTicks)
		{
			MaxAVG_RecastBuildCompressedLayers += GetMaxFromArray(IncAVGArray_RecastBuildCompressedLayers, k, k + MaxNumberOfTicks);
			MaxMAX_RecastBuildCompressedLayers += GetMaxFromArray(IncMAXArray_RecastBuildCompressedLayers, k, k + MaxNumberOfTicks);
			MaxAVG_RecastBuildNavigation += GetMaxFromArray(IncAVGArray_RecastBuildNavigation, k, k + MaxNumberOfTicks);
			MaxMAX_RecastBuildNavigation += GetMaxFromArray(IncMAXArray_RecastBuildNavigation, k, k + MaxNumberOfTicks);
			MaxAVG_Navigation_TickAsyncBuild += GetMaxFromArray(IncAVGArray_Navigation_TickAsyncBuild, k, k + MaxNumberOfTicks);
			MaxMAX_Navigation_TickAsyncBuild += GetMaxFromArray(IncMAXArray_Navigation_TickAsyncBuild, k, k + MaxNumberOfTicks);
			MaxAVG_Navigation_TickMarkDirty += GetMaxFromArray(IncAVGArray_Navigation_TickMarkDirty, k, k + MaxNumberOfTicks);
			MaxMAX_Navigation_TickMarkDirty += GetMaxFromArray(IncMAXArray_Navigation_TickMarkDirty, k, k + MaxNumberOfTicks);
			MaxAVG_DebugNavOctree += GetMaxFromArray(IncAVGArray_DebugNavOctree, k, k + MaxNumberOfTicks);
			MaxMAX_DebugNavOctree += GetMaxFromArray(IncMAXArray_DebugNavOctree, k, k + MaxNumberOfTicks);
			MaxAVG_AddingActorsToNavOctree += GetMaxFromArray(IncAVGArray_AddingActorsToNavOctree, k, k + MaxNumberOfTicks);
			MaxMAX_AddingActorsToNavOctree += GetMaxFromArray(IncMAXArray_AddingActorsToNavOctree, k, k + MaxNumberOfTicks);
		}


		FString MaxValues = LINE_TERMINATOR + FString::SanitizeFloat(MaxAVG_RecastBuildCompressedLayers) + "," + FString::SanitizeFloat(MaxMAX_RecastBuildCompressedLayers) +
			FString::SanitizeFloat(MaxAVG_RecastBuildNavigation) + "," + FString::SanitizeFloat(MaxMAX_RecastBuildNavigation) + "," +
			FString::SanitizeFloat(MaxAVG_Navigation_TickAsyncBuild) + "," + FString::SanitizeFloat(MaxMAX_Navigation_TickAsyncBuild) + "," +
			FString::SanitizeFloat(MaxAVG_Navigation_TickMarkDirty) + "," + FString::SanitizeFloat(MaxMAX_Navigation_TickMarkDirty) + "," +
			FString::SanitizeFloat(MaxAVG_DebugNavOctree) + "," + FString::SanitizeFloat(MaxMAX_DebugNavOctree) + "," +
			FString::SanitizeFloat(MaxAVG_AddingActorsToNavOctree) + "," + FString::SanitizeFloat(MaxMAX_AddingActorsToNavOctree) + LINE_TERMINATOR;

		Text += MaxValues;

		float NavMeshFinalValueAVG = MaxAVG_RecastBuildCompressedLayers +
			MaxAVG_RecastBuildNavigation +
			MaxAVG_Navigation_TickAsyncBuild +
			MaxAVG_Navigation_TickMarkDirty +
			MaxAVG_DebugNavOctree +
			MaxAVG_AddingActorsToNavOctree;

		float NavMeshFinalValueMAX = MaxMAX_RecastBuildCompressedLayers +
			MaxMAX_RecastBuildNavigation +
			MaxMAX_Navigation_TickAsyncBuild +
			MaxMAX_Navigation_TickMarkDirty +
			MaxMAX_DebugNavOctree +
			MaxMAX_AddingActorsToNavOctree;

		FString FinalString = "Final Value\nAVG: " + FString::SanitizeFloat(NavMeshFinalValueAVG) + "ms \tMAX: " + FString::SanitizeFloat(NavMeshFinalValueMAX) + "ms";
		Text += FinalString;
	}

	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_RecastBuildCompressedLayers.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_RecastBuildCompressedLayers.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_RecastBuildNavigation.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_RecastBuildNavigation.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_Navigation_TickAsyncBuild.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_Navigation_TickAsyncBuild.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_Navigation_TickMarkDirty.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_Navigation_TickMarkDirty.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_DebugNavOctree.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_DebugNavOctree.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_AddingActorsToNavOctree.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_AddingActorsToNavOctree.Num());


	return FFileHelper::SaveStringToFile(Text, *SaveDirectory);
}


// It return a file that rappresents a navigation data
void APerformanceProfiler::DebugNavigationData(FString Filename) const
{
	FString SaveDirectory = FPaths::ProjectContentDir();

	class UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSys)
	{
		for (ANavigationData* NavData : NavSys->NavDataSet)
		{
			if (const FNavDataGenerator* Generator = NavData->GetGenerator())
			{
				Generator->ExportNavigationData(FString::Printf(TEXT("%s/%s"), *SaveDirectory, *NavData->GetName()));
			}
		}

		for (ANavigationData* NavData : NavSys->NavDataSet)
		{
			if (const FNavDataGenerator* Generator = NavData->GetGenerator())
			{
				UE_LOG(LogTemp, Error, TEXT("MEM USED: %d"), Generator->LogMemUsed());
			}
		}
	}
}


// Return the max value from an array 
float APerformanceProfiler::GetMaxFromArray(const TArray<float>& Array, int StartPos, int EndPos) const
{
	float MaxValue = 0.f;

	for (int i = StartPos; i < EndPos; i++)
	{
		if (Array[i] > MaxValue)
		{
			MaxValue = Array[i];
		}
	}

	return MaxValue;
}


//Reset tick, called when we want to record the stats
void APerformanceProfiler::ResetTick()
{
	CurrentTick = 0;
}