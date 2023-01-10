#include "LobbyGM.h"
#include "LobbyPS.h"

// Methods
void ALobbyGM::TravelSeamless(const FString& MapName)
{
    bUseSeamlessTravel = true;
	GetWorld()->ServerTravel(MapName);
}

// Overriden function
void ALobbyGM::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator();
         Iterator;
         Iterator++)
	{
		ActorList.Add(Iterator->Get()->GetPlayerState<ALobbyPS>());
	}

	Super::GetSeamlessTravelActorList(bToTransition, ActorList);
}