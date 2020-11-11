// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VrNavigationDemoGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class VRNAVIGATIONDEMO_API AVrNavigationDemoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	

protected:
	

public:
	AVrNavigationDemoGameModeBase();

	// Called when the game starts
	virtual void StartPlay() override;


	//virtual void Tick(float DeltaTime) override;
	
};
