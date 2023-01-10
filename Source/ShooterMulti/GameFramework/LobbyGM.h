#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGM.generated.h"

UCLASS()
class SHOOTERMULTI_API ALobbyGM : public AGameModeBase
{
	GENERATED_BODY()

public:
    // Meant to be set once from blueprint
    UPROPERTY(BlueprintReadWrite)
    int32 MaxPlayer;

    UFUNCTION(BlueprintCallable)
    void TravelSeamless(const FString& MapName);

    // Overriden function
    void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) final;
};
