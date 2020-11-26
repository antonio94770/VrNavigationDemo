// Fill out your copyright notice in the Description page of Project Settings.


#include "PerformanceProfiler.h"
#include "Stats\StatsData.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"

// Sets default values
APerformanceProfiler::APerformanceProfiler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APerformanceProfiler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APerformanceProfiler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (CurrentTick < MaxNumberOfTicks)
	{
		//UE_LOG(LogTemp, Error, TEXT("STO PROFILANDO"));
		GetAllProfilingData();
		CurrentTick++;
	}
}


void APerformanceProfiler::GetAllProfilingData()
{
	GetNavigationProfilingData("STAT_Navigation_RecastBuildCompressedLayers", &IncAVGArray_RecastBuildCompressedLayers, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_RecastBuildCompressedLayers", &IncMAXArray_RecastBuildCompressedLayers, EComplexStatField::IncMax);
	GetNavigationProfilingData("STAT_Navigation_RecastBuildLayers", &IncAVGArray_RecastBuildLayers, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_RecastBuildLayers", &IncMAXArray_RecastBuildLayers, EComplexStatField::IncMax); 
	GetNavigationProfilingData("STAT_Navigation_RecastBuildNavigation", &IncAVGArray_Navigation_BuildTime, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_RecastBuildNavigation", &IncMAXArray_Navigation_BuildTime, EComplexStatField::IncMax);
	GetNavigationProfilingData("STAT_Navigation_GenerateNavigationDataLayer", &IncAVGArray_Navigation_GenerateNavigationDataLayer, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_GenerateNavigationDataLayer", &IncMAXArray_Navigation_GenerateNavigationDataLayer, EComplexStatField::IncMax);
	//GetNavigationProfilingData("STAT_Navigation_RecastMemory", &IncAVGArray_Navigation_RecastMemory, EComplexStatField::Num);
	//GetNavigationProfilingData("STAT_Navigation_RecastMemory", &IncMAXArray_Navigation_RecastMemory, EComplexStatField::ExcAve);
	GetNavigationProfilingData("STAT_Navigation_RecastBuildRegions", &IncAVGArray_RecastBuildRegions, EComplexStatField::IncAve);
	GetNavigationProfilingData("STAT_Navigation_RecastBuildRegions", &IncMAXArray_RecastBuildRegions, EComplexStatField::IncMax);
	/*GetNavigationProfilingData("STAT_Navigation_RecastBuildRegions");
	GetNavigationProfilingData("STAT_Navigation_TickAsyncBuild");
	//Manca Recast: create filters
	GetNavigationProfilingData("STAT_Navigation_RecastBuildCompactHeightField");
	GetNavigationProfilingData("STAT_Navigation_RecastBuildPolyDetail");
	GetNavigationProfilingData("STAT_Navigation_FilterLedgeSpans");
	GetNavigationProfilingData("STAT_Navigation_RecastRasterizeTriangles");
	GetNavigationProfilingData("STAT_Navigation_RecastErodeWalkable");
	GetNavigationProfilingData("STAT_Navigation_RecastBuildTileCache");
	GetNavigationProfilingData("STAT_Navigation_RecastBuildContours");
	GetNavigationProfilingData("STAT_Navigation_TickMarkDirty");
	GetNavigationProfilingData("STAT_Navigation_RecastCreateHeightField");*/
	//UE_LOG
}


void APerformanceProfiler::GetNavigationProfilingData(FString Stat, TArray<FString>* Array, EComplexStatField::Type DataType)
{
	FGameThreadStatsData* ViewData = FLatestGameThreadStatsData::Get().Latest;
	if (ViewData != nullptr)
	{
		if (ViewData->NameToStatMap.FindRef(FName(Stat)))
		{
			auto stat_data = ViewData->NameToStatMap.FindRef(FName(Stat))->GetValue_int64(DataType);
			Array->Add(FString::FromInt(stat_data / 10000));
		}			
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Start STAT Navigation first!!"));
		Array->Add("0");
	}

}


bool APerformanceProfiler::SaveArrayToFile(FString Filename, bool AllowOverWriting /*= true*/)
{
	FString SaveDirectory = FPaths::ProjectContentDir() + "\\" + Filename;

	UE_LOG(LogTemp, Error, TEXT("Path: %s"), *SaveDirectory);


	if (!AllowOverWriting)
	{
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*SaveDirectory))
		{
			return false;
		}
	}

	FString Text;
	Text = "IncAVG_RecastBuildCompressedLayers,\
			IncMAX_RecastBuildCompressedLayers,\
			IncAVG_RecastBuildLayers,\
			IncMAX_RecastBuildLayers,\
			IncAVGArray_Navigation_BuildTime,\
			IncMAXArray_Navigation_BuildTime,\
			IncAVGArray_Navigation_GenerateNavigationDataLayer,\
			IncMAXArray_Navigation_GenerateNavigationDataLayer,\
			IncAVGArray_RecastBuildRegions,\
			IncMAXArray_RecastBuildRegions";


			/*STAT_Navigation_RecastMemory\
			STAT_Navigation_RecastMemory"\*/
	Text += LINE_TERMINATOR;

	
	FString FinalString = "";

	if (IncAVGArray_RecastBuildCompressedLayers.Num() == MaxNumberOfTicks &&
		IncMAXArray_RecastBuildCompressedLayers.Num() == MaxNumberOfTicks &&
		IncAVGArray_RecastBuildLayers.Num() == MaxNumberOfTicks &&
		IncMAXArray_RecastBuildLayers.Num() == MaxNumberOfTicks &&
		IncAVGArray_Navigation_BuildTime.Num() == MaxNumberOfTicks &&
		IncMAXArray_Navigation_BuildTime.Num() == MaxNumberOfTicks &&
		IncAVGArray_Navigation_GenerateNavigationDataLayer.Num() == MaxNumberOfTicks &&
		IncMAXArray_Navigation_GenerateNavigationDataLayer.Num() == MaxNumberOfTicks &&
		IncAVGArray_RecastBuildRegions.Num() == MaxNumberOfTicks &&
		IncMAXArray_RecastBuildRegions.Num())
	{
		for (int i = 0; i < CurrentTick; i++)
		{
			Text += IncAVGArray_RecastBuildCompressedLayers[i] + "," +
				IncMAXArray_RecastBuildCompressedLayers[i] + "," +
				IncAVGArray_RecastBuildLayers[i] + "," +
				IncMAXArray_RecastBuildLayers[i] + "," +
				IncAVGArray_Navigation_BuildTime[i] + "," +
				IncMAXArray_Navigation_BuildTime[i] + "," +
				IncAVGArray_Navigation_GenerateNavigationDataLayer[i] + "," +
				IncMAXArray_Navigation_GenerateNavigationDataLayer[i] + "," +
				IncAVGArray_RecastBuildRegions[i] + "," +
				IncMAXArray_RecastBuildRegions[i] +
				LINE_TERMINATOR;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_RecastBuildCompressedLayers.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_RecastBuildCompressedLayers.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_RecastBuildLayers.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_RecastBuildLayers.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_Navigation_BuildTime.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_Navigation_BuildTime.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_Navigation_GenerateNavigationDataLayer.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_Navigation_GenerateNavigationDataLayer.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncAVGArray_RecastBuildRegions.Num());
	UE_LOG(LogTemp, Error, TEXT("SIZE ARRAY: %d"), IncMAXArray_RecastBuildRegions.Num());


	return FFileHelper::SaveStringToFile(Text, *SaveDirectory);
}


void APerformanceProfiler::ResetTick()
{
	CurrentTick = 0;
}