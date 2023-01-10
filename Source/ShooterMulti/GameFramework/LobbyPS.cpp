#include "LobbyPS.h"
#include "LobbyGS.h"

#include "Net/UnrealNetwork.h"

ALobbyPS::ALobbyPS()
{
    bReplicates = true;
}

void ALobbyPS::OnRep_PlayerName()
{
    OnRep_PlayerName_Implementation();
}

void ALobbyPS::SetIsReady(bool Toggle)
{
    // Prevent pointless replication
    if (bIsReady != Toggle)
    {
        bIsReady = Toggle;

        ALobbyGS* const LobbyGS = GetWorld()->GetGameState<ALobbyGS>();
        LobbyGS->UpdatePlayerReadyCount(Toggle ? 1 : -1);
    }
}

void ALobbyPS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALobbyPS, TeamNum);
    DOREPLIFETIME(ALobbyPS, bIsReady);
}