#include "LobbyGS.h"
#include "LobbyGM.h"
#include "Net/UnrealNetwork.h"

void ALobbyGS::BeginPlay()
{
    LobbyGM = GetWorld()->GetAuthGameMode<ALobbyGM>();
}

// Called server-side only
void ALobbyGS::UpdatePlayerReadyCount(int32 Offset)
{
    checkf(LobbyGM != nullptr,
           TEXT("ALobbyGS::UpdatePlayerReadyCount() must only be executed server-side"));

    PlayerReadyCount += Offset;

    if (PlayerReadyCount == LobbyGM->MaxPlayer)
    {
        OnAllPlayersReady();
    }
}

bool ALobbyGS::IsTeamFull(int32 PlayerCountInTeam) const
{
    checkf(LobbyGM != nullptr,
           TEXT("ALobbyGS::UpdatePlayerReadyCount() must only be executed server-side"));
           
    return PlayerCountInTeam == (LobbyGM->MaxPlayer / 2);
}


bool ALobbyGS::CanStartReadyPoll() const
{
    return (LobbyGM->MaxPlayer == (RedTeamCount + BlueTeamCount));
}

void ALobbyGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALobbyGS, RedTeamCount);
    DOREPLIFETIME(ALobbyGS, BlueTeamCount);
    DOREPLIFETIME(ALobbyGS, PlayerReadyCount);
}