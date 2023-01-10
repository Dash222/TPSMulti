
#pragma once

#include "GameFramework/Actor.h"
#include "../Characters/UndeadCharacter.h"
#include "../GameFramework/Resetable.h"
#include "EnemySpawnerButton.generated.h"

UCLASS()
class SHOOTERMULTI_API AEnemySpawnerButton : public AActor, public IResetable
{
	GENERATED_BODY()
	
protected:

	ETeam mTeam = ETeam::None;
	FTimerHandle mSpawnTimerHandle;

	virtual void BeginPlay() override;
	

public:

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly)
	float SecondPerSpawn = 7.0f;

	UPROPERTY(EditAnywhere)
	USoundBase* ActivateSound;

	UMaterialInstanceDynamic* material;

	AEnemySpawnerButton();

	void Activate(ETeam team);

	UFUNCTION()
	void SetTeam(ETeam team);

	UFUNCTION()
	void Reset() override;
};
