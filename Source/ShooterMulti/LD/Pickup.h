// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "../GameFramework/Resetable.h"
#include "Pickup.generated.h"


USTRUCT()
struct FSpawnKey
{
	GENERATED_BODY()

	int ClassKey = 0;
	int SpawnPointKey = 0;
};

UCLASS()
class SHOOTERMULTI_API APickup : public AActor, public IResetable
{
	GENERATED_BODY()

private:

	FTimerHandle CoolDownTimerHandle;

	float GlobalTime = 0.0f;

	friend class APickupDirector;
	FSpawnKey SpawnKey;

protected:

	APickup();

	UPROPERTY(EditAnywhere, Category = "Pickup")
	int CoolDownDuration = 10;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	int RotationSpeed = 42;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	USoundBase* PickupSound;

	void Tick(float DeltaTime) override;

	void NotifyActorBeginOverlap(AActor * OtherActor) override;

	void Reset() override;
};