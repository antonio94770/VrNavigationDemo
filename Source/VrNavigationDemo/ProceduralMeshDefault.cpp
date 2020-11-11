// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralMeshDefault.h"

#include "ProceduralMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AProceduralMeshDefault::AProceduralMeshDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	SetRootComponent(ProceduralMesh);

}

// Called when the game starts or when spawned
void AProceduralMeshDefault::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AProceduralMeshDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

