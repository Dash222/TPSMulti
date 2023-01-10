#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPS.generated.h"

UCLASS()
class SHOOTERMULTI_API ALobbyPS : public APlayerState
{
	GENERATED_BODY()

public:
    UPROPERTY(ReplicatedUsing=OnRep_TeamNum, BlueprintReadWrite)
    int32 TeamNum = -1;

    UPROPERTY(ReplicatedUsing=OnRep_IsReady, BlueprintReadWrite)
    bool bIsReady = false;

    ALobbyPS();

    // Despite being virtual and public, OnRep_PlayerName() cannot be overriden from blueprints
    UFUNCTION(BlueprintImplementableEvent)
    void OnRep_PlayerName_Implementation();
    void OnRep_PlayerName() override;

    UFUNCTION(BlueprintImplementableEvent)
    void OnRep_TeamNum();

    UFUNCTION(BlueprintImplementableEvent)
    void OnRep_IsReady();

    UFUNCTION(BlueprintCallable)
    void SetIsReady(bool Toggle);

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const final;
};