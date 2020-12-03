// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
#include "MotionControllerComponent.h"
#include "ProceduralMeshWithBoxCollider.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "ProceduralMeshDefault.h"
#include <VrNavigationDemo\NavMeshController.h>
#include "OptimizeNavMeshScene.h"
#include "PerformanceProfiler.h"
#include "EngineUtils.h"


// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootVR  = CreateDefaultSubobject<USceneComponent>(TEXT("RootVR"));
	RootVR->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	Camera->SetupAttachment(RootVR);

	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController -> SetupAttachment(RootVR);
	LeftController->SetTrackingSource(EControllerHand::Left);
	LeftController->bDisplayDeviceModel = true;

	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(RootVR);
	RightController->SetTrackingSource(EControllerHand::Right);
	RightController->bDisplayDeviceModel = true;

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	TeleportPath->SetupAttachment(RightController);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());

	CurrentFloorForTesting = 0;

}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	DestinationMarker->SetVisibility(false);
	bCanTeleport = false;
	bAlreadyTeleported = false;

	NavController = NavMeshController(GetWorld(), NavMeshType, bOptimizeNavMesh);
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	RootVR->AddWorldOffset(-NewCameraOffset);

	//UE_LOG(LogTemp, Error, TEXT("Rotation: %s: "), *Camera->GetRelativeRotation().ToString());

	UpdateDestinationMarker();
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::ForwardMovement);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::RightMovement);
	PlayerInputComponent->BindAction(TEXT("Teleport"), IE_Released, this, &AVRCharacter::StartTeleport);
	PlayerInputComponent->BindAction(TEXT("SpawnProceduralMesh"), IE_Released, this, &AVRCharacter::SpawnProceduralMesh);
	PlayerInputComponent->BindAction(TEXT("SpawnProceduralMeshWithCollision"), IE_Released, this, &AVRCharacter::SpawnProceduralMeshWithCollision);
	PlayerInputComponent->BindAction(TEXT("SpawnDefaultProceduralMesh"), IE_Released, this, &AVRCharacter::SpawnDefaultProceduralMesh);
	PlayerInputComponent->BindAction(TEXT("SpawnNavMesh"), IE_Released, this, &AVRCharacter::SpawnNavMesh);
	PlayerInputComponent->BindAction(TEXT("SaveToFile"), IE_Released, this, &AVRCharacter::SaveToFileForStudyingPerformance);
	//TO DO: implementare spawn procedural mesh normali
}


bool AVRCharacter::FindTeleportDestination(TArray<FVector>& OutPath, FVector& OutLocation)
{
	FVector Start;
	FVector End;
	FVector Look;

	if (!bCanUseMouseInputForCamera)
	{
		Start = RightController->GetComponentLocation();
		Look = RightController->GetForwardVector();
	}
	else
	{
		Start = Camera->GetComponentLocation();
		Look = Camera->GetForwardVector();
	}

	FPredictProjectilePathParams ProjectileParams(
		TeleportProjectileRadius,
		Start,
		Look * TeleportProjectileSpeed,
		TeleportSimulationTime,
		ECC_Visibility,
		this
	);
	
	ProjectileParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	//ProjectileParams.bTraceComplex = true;

	FPredictProjectilePathResult ProjectileResult;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, ProjectileParams, ProjectileResult);

	/*DrawDebugLine(
		GetWorld(),
		Start,
		End,
		FColor(255, 0, 0),
		false, -1, 0,
		12.333
	);*/

	if (!bHit) return false;

	for (FPredictProjectilePathPointData PointData : ProjectileResult.PathData)
	{
		OutPath.Add(PointData.Location);
	}

	UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());

	FNavLocation NavLocation;
	bool bOnNavSMesh = false;
	if (NavSystem != nullptr) {
		bOnNavSMesh = NavSystem->ProjectPointToNavigation(ProjectileResult.HitResult.Location, NavLocation, TeleportExtent);
	}

	if (!bOnNavSMesh) return false;

	OutLocation = NavLocation.Location;
	return bOnNavSMesh && bHit;
}


void AVRCharacter::UpdateDestinationMarker()
{
	
	FVector Location;
	TArray<FVector> Path;

	bCanTeleport = FindTeleportDestination(Path, Location);

	if (bCanTeleport)
	{
		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(Location);
		if(!bAlreadyTeleported)
			LastUsefullPositionForTeleport = DestinationMarker->GetComponentLocation();

		DrawTeleportPath(Path);
	}
	else
	{
		DestinationMarker->SetVisibility(false);

		TArray<FVector> EmptyPath;
		DrawTeleportPath(EmptyPath);
	}
}


void AVRCharacter::StartCameraFade(float From, float To)
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC != nullptr)
	{
		PC->PlayerCameraManager->StartCameraFade(From, To, TeleportFadeTime, FLinearColor::Black);
	}
}


void AVRCharacter::StartTeleport()
{
	if (bCanTeleport && !bAlreadyTeleported)
	{
		bAlreadyTeleported = true;
		UE_LOG(LogTemp, Error, TEXT("Teleported."));
		StartCameraFade(0, 1);

		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::EndTeleport, TeleportFadeTime);
	}
}


void AVRCharacter::EndTeleport()
{
	FVector Destination = LastUsefullPositionForTeleport;
	Destination += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * GetActorUpVector();

	SetActorLocation(Destination);

	StartCameraFade(1, 0);
	bAlreadyTeleported = false;
}


void AVRCharacter::UpdateSpline(const TArray<FVector>& Path)
{
	TeleportPath->ClearSplinePoints(false);
	for (int32 i = 0; i < Path.Num(); i++)
	{
		//Transform world position to local position
		FVector LocalPosition = TeleportPath->GetComponentTransform().InverseTransformPosition(Path[i]);
		FSplinePoint Point(i, LocalPosition, ESplinePointType::Curve);
		TeleportPath->AddPoint(Point, false);
	}

	TeleportPath->UpdateSpline();
}


void AVRCharacter::DrawTeleportPath(const TArray<FVector>& Path)
{
	UpdateSpline(Path);

	//First all SplineMeshComponent are invisible
	for (USplineMeshComponent* SplineMesh : TeleportMeshPool)
	{
		SplineMesh->SetVisibility(false);
	}

	//For all segment we create new SplineMeshComponent with a mesh and a meterial and we add it to our array of SplineMeshComponent
	int32 SegmentNumber = Path.Num() - 1;
	for (int32 i = 0; i < SegmentNumber; i++)
	{
		//To check if already we have that SplineMesh and so we don't have to recreate it
		if (TeleportMeshPool.Num() <= i)
		{
			USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
			SplineMesh->SetMobility(EComponentMobility::Movable);
			SplineMesh->AttachToComponent(TeleportPath, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMesh->SetStaticMesh(TeleportTrajectoryMesh);
			SplineMesh->SetMaterial(0, TeleportTrajectoryMaterial);
			SplineMesh->RegisterComponent();

			TeleportMeshPool.Add(SplineMesh);
		}
		
		//Get here the element for our pool and we set it visible
		USplineMeshComponent* SplineMesh = TeleportMeshPool[i];
		SplineMesh->SetVisibility(true);
		
		//Get points and tangents vectors for all points in the path
		FVector StartPos, EndPos, StartTangent, EndTangent;
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i, StartPos, StartTangent);
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTangent);

		//With the true at the end with update the SplineMesh (it works also without it because is a default value)
		SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
	}
}


void AVRCharacter::SpawnProceduralMesh()
{
	FVector Location;
	TArray<FVector> Path;
	if (BP_ProceduralMeshWithBoxCollider != nullptr && FindTeleportDestination(Path, Location))
	{
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		GetWorld()->SpawnActor<AProceduralMeshWithBoxCollider>(BP_ProceduralMeshWithBoxCollider, LastUsefullPositionForTeleport, Rotation, SpawnInfo);
	}
}

void AVRCharacter::SpawnProceduralMeshWithCollision()
{
	FVector Location;
	TArray<FVector> Path;
	if (BP_ProceduralMeshWithBoxColliderWithCollision != nullptr && FindTeleportDestination(Path, Location))
	{
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		GetWorld()->SpawnActor<AProceduralMeshWithBoxCollider>(BP_ProceduralMeshWithBoxColliderWithCollision, LastUsefullPositionForTeleport, Rotation, SpawnInfo);
	}
}


void AVRCharacter::SpawnDefaultProceduralMesh()
{
	FVector Location;
	TArray<FVector> Path;
	if (BP_ProceduralMeshDefault != nullptr && FindTeleportDestination(Path, Location))
	{
		UE_LOG(LogTemp, Error, TEXT("SPAWN"));

		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		auto ActorSpawned = GetWorld()->SpawnActor<AProceduralMeshDefault>(BP_ProceduralMeshDefault, LastUsefullPositionForTeleport + FVector(0.f, 0.f, 50.f), Rotation, SpawnInfo);
		OptimizeNavMeshScene Optimizer = OptimizeNavMeshScene(GetWorld());
		Optimizer.OptimizeSingleNavMeshActor(ActorSpawned);
	}
}


void AVRCharacter::SpawnNavMesh()
{
	NavController.ChangeCurrentFloor(CurrentFloorForTesting);
	NavController.RefreshNavMeshBounds();

	if (NavMeshType == ENavMeshTypeController::ONEFLOOR)
	{
		if (CurrentFloorForTesting < NavController.FloorsNumber - 1)
			CurrentFloorForTesting++;
		else
			CurrentFloorForTesting = 0;
	}		
}


void AVRCharacter::SaveToFileForStudyingPerformance()
{
	for (TActorIterator<APerformanceProfiler> ActorItr(GetWorld()); ActorItr; ++ActorItr) {

		if (ActorItr->MaxNumberOfTicks == ActorItr->CurrentTick)
		{
			if (bOptimizeNavMesh)
			{
				if (NavMeshType == ENavMeshTypeController::SINGLEMODE)
					ActorItr->SaveArrayToFile("SingleModeOptimization.csv", true);

				if (NavMeshType == ENavMeshTypeController::ONEFLOOR)
					ActorItr->SaveArrayToFile("OneFloorOptimization.csv", true);

				if (NavMeshType == ENavMeshTypeController::MULTIPLEFLOOR)
					ActorItr->SaveArrayToFile("MultipleFloorOptimization.csv", true);
			}
			else
			{
				if (NavMeshType == ENavMeshTypeController::SINGLEMODE)
					ActorItr->SaveArrayToFile("SingleModeNoOptimization.csv", true);

				if (NavMeshType == ENavMeshTypeController::ONEFLOOR)
					ActorItr->SaveArrayToFile("OneFloorNoOptimization.csv", true);

				if (NavMeshType == ENavMeshTypeController::MULTIPLEFLOOR)
					ActorItr->SaveArrayToFile("MultipleFloorNoOptimization.csv", true);
			}
		}
	}
}

void AVRCharacter::ForwardMovement(float moveSpeed)
{
	AddMovementInput(moveSpeed * Camera->GetForwardVector());
}

void AVRCharacter::RightMovement(float moveSpeed)
{
	AddMovementInput(moveSpeed * Camera->GetRightVector());
}