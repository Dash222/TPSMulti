#include "EnemySpawnerButton.h"
#include "../AI/UndeadDirector.h"
#include "../GameFramework/DeathMatchGS.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawnerButton::AEnemySpawnerButton()
{
}

void AEnemySpawnerButton::BeginPlay()
{
	Super::BeginPlay();

	if (material == nullptr)
	{
		TArray<UStaticMeshComponent*> Components;
		GetComponents<UStaticMeshComponent>(Components);
		material = Components[0]->CreateAndSetMaterialInstanceDynamic(2);
	}
}

void AEnemySpawnerButton::Activate(ETeam team)
{
	if (mTeam == team)
		return;
	
	SetTeam(team);

	auto lambda = [this]()
	{
		auto dir = AUndeadDirector::GetInstance();
		dir->SpawnEnemy(GetActorLocation(), GetActorRotation(), mTeam);
	};

	GetWorld()->GetTimerManager().SetTimer(mSpawnTimerHandle, lambda, SecondPerSpawn, true);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ActivateSound, GetActorLocation());
}

void AEnemySpawnerButton::SetTeam(ETeam team)
{
	mTeam = team;

	if (!material)
		return;

	FLinearColor color = mTeam == ETeam::Blue ? FLinearColor::Blue : mTeam == ETeam::Red ? FLinearColor::Red : FLinearColor::Green;

	material->SetVectorParameterValue("ColorActive", color);
}

void AEnemySpawnerButton::Reset()
{
	SetTeam(ETeam::None);

	GetWorldTimerManager().ClearTimer(mSpawnTimerHandle);
}
