// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
#include "MotionControllerComponent.h"
#include "ProceduralMeshWithBoxCollider.h"
#include "ProceduralMeshActor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "ProceduralMeshDefault.h"
#include <VrNavigationDemo\NavMeshController.h>
#include "OptimizeNavMeshScene.h"
#include "PerformanceProfiler.h"
#include "NavModifierComponent.h"
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

	UpdateDestinationMarker();
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::ForwardMovement);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::RightMovement);
	PlayerInputComponent->BindAction(TEXT("Teleport"), IE_Released, this, &AVRCharacter::StartTeleport);
	PlayerInputComponent->BindAction(TEXT("SpawnProceduralMeshNavModifier"), IE_Released, this, &AVRCharacter::SpawnProceduralMeshNavModifier);
	PlayerInputComponent->BindAction(TEXT("SpawnDefaultProceduralMesh"), IE_Released, this, &AVRCharacter::SpawnDefaultProceduralMesh);
	PlayerInputComponent->BindAction(TEXT("SpawnOptimizedProceduralMesh"), IE_Released, this, &AVRCharacter::SpawnOptimizedProceduralMesh);
	PlayerInputComponent->BindAction(TEXT("SpawnNavMesh"), IE_Released, this, &AVRCharacter::SpawnNavMesh);
	PlayerInputComponent->BindAction(TEXT("SaveToFile"), IE_Released, this, &AVRCharacter::SaveToFileForStudyingPerformance);
	//TO DO: implementare spawn procedural mesh normali
}


/* Get our projectile trajectory based on vr or mouse, and we check if it points to a navmesh layer */
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
	
	//ProjectileParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
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

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	FNavLocation NavLocation;
	bool bOnNavSMesh = false;
	if (NavSystem != nullptr) {
		bOnNavSMesh = NavSystem->ProjectPointToNavigation(ProjectileResult.HitResult.Location, NavLocation, TeleportExtent);
	}

	if (!bOnNavSMesh) return false;

	OutLocation = NavLocation.Location;
	return bOnNavSMesh && bHit;
}


/* Update our extent position and visibility, and our path, if we point to a navmesh layer */
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


/* Manage the start of a teleport */
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


/* Fade of the camera when teleport start and end */
void AVRCharacter::StartCameraFade(float From, float To)
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC != nullptr)
	{
		PC->PlayerCameraManager->StartCameraFade(From, To, TeleportFadeTime, FLinearColor::Black);
	}
}


/* Manage the end of a teleport (Camera fade)*/
void AVRCharacter::EndTeleport()
{
	FVector Destination = LastUsefullPositionForTeleport;
	Destination += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * GetActorUpVector();

	SetActorLocation(Destination);

	StartCameraFade(1, 0);
	bAlreadyTeleported = false;
}


/* We add path points to USplineComponent */
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


/* Update and draw of the path*/
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


/* ONLY FOR TESTING: Spawn actor with NavModifier Component */
void AVRCharacter::SpawnProceduralMeshNavModifier()
{
	FVector Location;
	TArray<FVector> Path;
	if (BP_ProceduralMeshNavModifier != nullptr && FindTeleportDestination(Path, Location))
	{
		for (TActorIterator<APerformanceProfiler> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			ActorItr->ResetTick();
		}

		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		auto ActorSpawned = GetWorld()->SpawnActor<AProceduralMeshActor>(BP_ProceduralMeshNavModifier, FVector(0.f, 0.f, 0.f), Rotation, SpawnInfo);
		ActorSpawned->SetActorRelativeScale3D(FVector(5.f, 3.f, 2.f));

		TArray<UNavModifierComponent*> NavModifiers;
		ActorSpawned->GetComponents(NavModifiers);

		for (auto elem : NavModifiers)
		{
			FVector Origin;
			FVector BoxExtent;
			ActorSpawned->GetActorBounds(false, Origin, BoxExtent);
			elem->FailsafeExtent = BoxExtent;
		}

		bSpawnedMeshNavModifier = true;
	}
}


/* ONLY FOR TESTING: Spawn Default actor */
void AVRCharacter::SpawnDefaultProceduralMesh()
{
	FVector Location;
	TArray<FVector> Path;
	if (BP_ProceduralMeshDefault != nullptr && FindTeleportDestination(Path, Location))
	{
		for (TActorIterator<APerformanceProfiler> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			ActorItr->ResetTick();
		}

		UE_LOG(LogTemp, Error, TEXT("SPAWN"));

		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		auto ActorSpawned = GetWorld()->SpawnActor<AProceduralMeshDefault>(BP_ProceduralMeshDefault, FVector(0.f, 0.f, 0.f), Rotation, SpawnInfo);
		ActorSpawned->SetActorRelativeScale3D(FVector(5.f, 3.f, 2.f));
		bSpawnedDefaultMesh = true;
	}
}


/* ONLY FOR TESTING: Spawn Optimized actor */
void AVRCharacter::SpawnOptimizedProceduralMesh()
{
	FVector Location;
	TArray<FVector> Path;
	if (BP_ProceduralMeshOptimized != nullptr && FindTeleportDestination(Path, Location))
	{
		for (TActorIterator<APerformanceProfiler> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			ActorItr->ResetTick();
		}

		UE_LOG(LogTemp, Error, TEXT("SPAWN"));

		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;

		auto ActorSpawned = GetWorld()->SpawnActor<AProceduralMeshDefault>(BP_ProceduralMeshOptimized, FVector(0.f, 0.f, 0.f), Rotation, SpawnInfo);
		ActorSpawned->SetActorRelativeScale3D(FVector(5.f,3.f,2.f));
		OptimizeNavMeshScene Optimizer = OptimizeNavMeshScene(GetWorld());
		Optimizer.OptimizeSingleNavMeshActor(ActorSpawned);
		bSpawnedOptimizedMesh = true;
	}
}


/* Used for testing the navmesh and we increase the floor number*/
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


/* Function to save stats output to a file based on optimization and on navmesh type */
void AVRCharacter::SaveToFileForStudyingPerformance()
{
	for (TActorIterator<APerformanceProfiler> ActorItr(GetWorld()); ActorItr; ++ActorItr) {

		if (ActorItr->MaxNumberOfTicks == ActorItr->CurrentTick)
		{
			if (bSpawnedMeshNavModifier)
			{
				ActorItr->SaveArrayToFile("SpawnProceduralMeshNavModifier.csv", true);
				bSpawnedMeshNavModifier = false;
			}
			else if (bSpawnedDefaultMesh)
			{
				ActorItr->SaveArrayToFile("SpawnDefaultProceduralMesh.csv", true);
				bSpawnedDefaultMesh = false;
			}
			else if (bSpawnedOptimizedMesh)
			{
				ActorItr->SaveArrayToFile("SpawnOptimizedProceduralMesh.csv", true);
				bSpawnedOptimizedMesh = false;
			}
			else if (bOptimizeNavMesh)
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

/* Forward Movement with keyboard */
void AVRCharacter::ForwardMovement(float moveSpeed)
{
	AddMovementInput(moveSpeed * Camera->GetForwardVector());
}

/* Right Movement with keyboard */
void AVRCharacter::RightMovement(float moveSpeed)
{
	AddMovementInput(moveSpeed * Camera->GetRightVector());
}