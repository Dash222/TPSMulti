#pragma once

#include "Pickup.h"
#include "GameFramework/Actor.h"
#include "PickupDirector.generated.h"

UCLASS()
class SHOOTERMULTI_API APickupDirector : public AActor
{
	GENERATED_BODY()
	
private:

	static APickupDirector* Instance;

	TArray<bool> IsSpawnEmptyArray;
	TArray<int> PickupCountArray;

	FTimerHandle TimerHandle;

	bool bIsFull = false;

protected:

	virtual void BeginPlay() override;

public:

	APickupDirector();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta = (ClampMin = 0.1f))
	float SecondPerSpawn = 15.0f;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<TSubclassOf<APickup>> PickupBPs;
	
	void SpawnTick();
	static void FreePickup(FSpawnKey Key);

	void UpdateFrequencies(class ADeathMatchGS* GameState);
};