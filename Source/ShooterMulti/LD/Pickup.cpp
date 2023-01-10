#include "Pickup.h"
#include "PickupDirector.h"
#include "../Characters/ShooterCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void APickup::Tick(float DeltaTime)
{
	GlobalTime += DeltaTime;
	float Offset = sin(GlobalTime) / 2.0f;

	FVector NewPos = GetActorLocation();
	NewPos.Z += Offset;
	SetActorLocation(NewPos);

	FRotator NewRot = GetActorRotation();
	NewRot.Yaw += 42 * sin(DeltaTime);
	SetActorRotation(NewRot);
}

void APickup::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);

	if (!Player)
		return;

	//play the shot sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());

	APickupDirector::FreePickup(SpawnKey);
	
	Destroy();
}

void APickup::Reset()
{
	Destroy();
}
