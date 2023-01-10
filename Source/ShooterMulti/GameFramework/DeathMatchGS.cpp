#include "DeathMatchGS.h"
#include "ShooterPS.h"
#include "TimerManager.h"
#include "DeathMatchGM.h"
#include "../Characters/ShooterCharacter.h"
#include "../LD/Pickup.h"
#include "../Controllers/ShooterController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"


#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Cyan, text)

void ADeathMatchGS::BeginPlay()
{
    if (IsNetMode(NM_DedicatedServer))
    {
        Super::BeginPlay();

        OnTeamWin.AddLambda([this](ETeam Team) { ShowTeamWinHUD(Team); });

        OnGameRestart.AddLambda([this]() { Reset(); });

        GameMode = Cast<ADeathMatchGM>(AuthorityGameMode);

        check(GameMode && "GameMode nullptr: Cast as ADeathMatchGM failed.");

        CurrentTime = GameMode->GameTime;
        GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);
    }
}

void ADeathMatchGS::AdvanceTimer()
{
	--CurrentTime;
	
	if (CurrentTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		if (RedTeamScore < BlueTeamScore)
			UpdateEndHud(ETeam::Blue);
		else if (RedTeamScore > BlueTeamScore)
			UpdateEndHud(ETeam::Red);
		else
			UpdateEndHud(ETeam::None);
	}
}

void ADeathMatchGS::AddScore(ETeam Team)
{
    if (IsNetMode(NM_DedicatedServer))
    {
        if (Team == ETeam::Red && ++RedTeamScore == GameMode->MaxKill)
            UpdateEndHud(ETeam::Red);
        else if (Team == ETeam::Blue && ++BlueTeamScore == GameMode->MaxKill)
            UpdateEndHud(ETeam::Blue);
    }
}

void ADeathMatchGS::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	OnPlayerNum.Broadcast(this);
}

void ADeathMatchGS::RemovePlayerState(APlayerState* PlayerState)
{
	OnPlayerNum.Broadcast(this);

	Super::RemovePlayerState(PlayerState);
}

bool ADeathMatchGS::CanAddAI()
{
    if (IsNetMode(NM_DedicatedServer))
    {
        return Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->MaxAIPerPlayer* PlayerArray.Num() > CurrentAICount;
    }

    return false;
}

void ADeathMatchGS::AddAI()
{
	CurrentAICount++;
}

void ADeathMatchGS::RemoveAI()
{
	CurrentAICount--;
}

int ADeathMatchGS::GetNbplayer()
{
	return PlayerArray.Num();
}

int ADeathMatchGS::NewFrequency(int Sec)
{
	return Sec / sqrt(0.61 * PlayerArray.Num());
}

void ADeathMatchGS::UpdateEndHud(ETeam Team)
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	OnTeamWin.Broadcast(Team);
}

void ADeathMatchGS::Reset()
{
	TArray<AActor*> Resetables;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UResetable::StaticClass(), Resetables);

	for (auto& res : Resetables)
		Cast<IResetable>(res)->Reset();
}

void ADeathMatchGS::ResetAfterDelay()
{
    if (IsNetMode(NM_DedicatedServer))
    {
        CurrentTime = GameMode->GameTime;
        GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);

        RedTeamScore = 0;
        BlueTeamScore = 0;
        CurrentAICount = 0;

        OnResetAfterDelay.Broadcast();
    }
}

void ADeathMatchGS::EndGameTrigg()
{
	OnGameRestart.Broadcast();
}
