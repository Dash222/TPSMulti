#include "PlayerGI.h"
#include "ShooterPS.h"

UPlayerGI::UPlayerGI(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	// force blue team on offline mode $$$ to remove when implementing online mode
	UserInfo.TeamNum = 1;
}

FPlayerInfo UPlayerGI::GetUserInfo()
{
	return UserInfo;
}

void UPlayerGI::SetUserInfo(int32 InTeamNum, const FString& InUserName)
{
	UserInfo.TeamNum = InTeamNum;
	UserInfo.UserName = InUserName;
}

void UPlayerGI::SetUsername(const FString& InUserName)
{
	UserInfo.UserName = InUserName;
}

void UPlayerGI::SetTeamNum(int32 InTeamNum)
{
	UserInfo.TeamNum = InTeamNum;
}