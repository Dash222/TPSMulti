#include "ShooterPS.h"
#include "LobbyPS.h"
#include "PlayerGI.h"
#include "../Characters/ShooterCharacter.h"
#include "DeathMatchGS.h"

void AShooterPS::BeginPlay()
{
	Super::BeginPlay();

	ADeathMatchGS* GameState = GetWorld()->GetGameState<ADeathMatchGS>();
	GameState->OnResetAfterDelay.AddLambda([this]() { Reset(); });
}

void AShooterPS::CopyProperties(class APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (PlayerState)
	{
		AShooterPS* ShooterPlayerState = Cast<AShooterPS>(PlayerState);
        
		if (ShooterPlayerState)
		{
			ShooterPlayerState->NbKill  = NbKill;
			ShooterPlayerState->NbDeath = NbDeath;
			ShooterPlayerState->SetPlayerName(GetPlayerName());
		}
	}
}

void AShooterPS::OverrideWith(class APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	if (PlayerState)
	{
		if (const AShooterPS* const ShooterPlayerState = Cast<AShooterPS>(PlayerState))
		{
			NbKill   = ShooterPlayerState->NbKill;
			NbDeath  = ShooterPlayerState->NbDeath;

			SetPlayerName(ShooterPlayerState->GetPlayerName());
		}
        else if (const ALobbyPS* const LobbyPlayerState = Cast<ALobbyPS>(PlayerState))
        {
            NbKill  = 0;
            NbDeath = 0;
            TeamNum = LobbyPlayerState->TeamNum;
            SetPlayerName(LobbyPlayerState->GetPlayerName());
        }
	}
}

void AShooterPS::Reset()
{
	NbKill  = 0;
    NbDeath = 0;
}
