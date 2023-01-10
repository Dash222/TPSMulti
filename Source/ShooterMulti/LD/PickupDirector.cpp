#include "PickupDirector.h"
#include "Engine/World.h"
#include "../GameFramework/DeathMatchGS.h"

APickupDirector* APickupDirector::Instance = nullptr;

APickupDirector::APickupDirector()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void APickupDirector::BeginPlay()
{
	Super::BeginPlay();

	if (!Instance)
		Instance = this;

	PickupCountArray.SetNum(PickupBPs.Num());
	IsSpawnEmptyArray.SetNum(SpawnPoints.Num());
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APickupDirector::SpawnTick, SecondPerSpawn, true);

	ADeathMatchGS* GameState = Cast<ADeathMatchGS>(GetWorld()->GetGameState());
	GameState->OnPlayerNum.AddLambda([this](ADeathMatchGS* GS) { UpdateFrequencies(GS); });
}

void APickupDirector::SpawnTick()
{
	if (bIsFull || GetNetMode() == NM_Client)
		return;
	
	int MaxPoints = SpawnPoints.Num() - 1;
	int MaxPickupPBs = PickupBPs.Num() - 1;

	int RandomPoint = FMath::RandRange(0, MaxPoints);
	int RandomPickupBP = FMath::RandRange(0, MaxPickupPBs);
	
	int PrevPoint = RandomPoint;

	// if same spawnPoint take next one. if loop on all spawnPoints: return
	while (IsSpawnEmptyArray[RandomPoint])
	{
		RandomPoint = (RandomPoint + 1) % MaxPoints;
		if (RandomPoint == PrevPoint)
		{
			bIsFull = true;
			return;
		}
	}

	PrevPoint = RandomPickupBP;

	// if same pickup spawn next one. if loop on all pickups: return
	while (PickupCountArray[RandomPickupBP] > (SpawnPoints.Num() / PickupBPs.Num()) + 1)
	{
		RandomPickupBP = (RandomPickupBP + 1) % MaxPoints;
		if (RandomPickupBP == PrevPoint)
		{
			bIsFull = true;
			return;
		}
	}

	PickupCountArray[RandomPickupBP]++;
	IsSpawnEmptyArray[RandomPoint] = true;

	auto Pickup = GetWorld()->SpawnActor<APickup>(PickupBPs[RandomPickupBP], SpawnPoints[RandomPoint]->GetActorLocation(),
		SpawnPoints[RandomPoint]->GetActorRotation());

	Pickup->SpawnKey.ClassKey = RandomPickupBP;
	Pickup->SpawnKey.SpawnPointKey = RandomPoint;
}

void APickupDirector::FreePickup(FSpawnKey Key)
{
	if (!Instance)
		return;

	if (--(Instance->PickupCountArray[Key.ClassKey]) < 0)
		Instance->PickupCountArray[Key.ClassKey] = 0;

	Instance->IsSpawnEmptyArray[Key.SpawnPointKey] = false;
}

void APickupDirector::UpdateFrequencies(ADeathMatchGS* GameState)
{
	bIsFull = false;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APickupDirector::SpawnTick, GameState->NewFrequency(SecondPerSpawn), true);
}
