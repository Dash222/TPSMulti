#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGS.generated.h"

class ALobbyGM;

UCLASS()
class SHOOTERMULTI_API ALobbyGS : public AGameStateBase
{
	GENERATED_BODY()

private:
    ALobbyGM* LobbyGM;

    UPROPERTY(ReplicatedUsing=OnRep_PlayerReadyCount)
    int32 PlayerReadyCount = 0;

public:
    // Meant for blueprints only
    UPROPERTY(Replicated, BlueprintReadWrite)
    int32 RedTeamCount = 0;

    UPROPERTY(Replicated, BlueprintReadWrite)
    int32 BlueTeamCount = 0;

    void BeginPlay();

    // Called server-side only
    void UpdatePlayerReadyCount(int32 Offset);

    UFUNCTION(BlueprintCallable)
    bool IsTeamFull(int32 PlayerCountInTeam) const;

    UFUNCTION(BlueprintCallable)
    bool CanStartReadyPoll() const;

    UFUNCTION(BlueprintCallable)
    FORCEINLINE int32 GetPlayerReadyCount() const { return PlayerReadyCount; }

    // Blueprint
    UFUNCTION(BlueprintImplementableEvent)
    void OnRep_PlayerReadyCount();

    UFUNCTION(BlueprintImplementableEvent)
    void OnAllPlayersReady();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void OnTeamJoinedOrLeft();
};
