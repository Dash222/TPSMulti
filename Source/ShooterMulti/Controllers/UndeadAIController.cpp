#include "UndeadAIController.h"
#include "../ShooterMulti.h"
#include "../Characters/UndeadCharacter.h"
#include "Kismet/GameplayStatics.h"

void AUndeadAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree && BlackboardData)
	{
		UseBlackboard(BlackboardData, BlackboardComponent);

		RunBehaviorTree(BehaviorTree);
	}
	else
		UE_LOG(GLogShooterMulti, Warning, TEXT("Please edit the BehaviorTree and BlackboardData values in Blueprint"));
}

bool AUndeadAIController::Punch()
{
	if (AUndeadCharacter * UndeadCharacter = Cast<AUndeadCharacter>(GetCharacter()))
		return UndeadCharacter->Punch();

	return false;
}
