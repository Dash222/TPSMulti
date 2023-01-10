#include "ShooterCharacter.h"
#include "../Animations/ShooterCharacterAnim.h"
#include "../GameFramework/PlayerGI.h"
#include "../LD/EnemySpawnerButton.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Animation/AnimBlueprint.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

AShooterCharacter::AShooterCharacter()
{
	DisapearingDelay = 1.5f;

	// Animation is set in ShooterCharacter_BP to fix build.
	//// Set Animations
	//ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimContainer(TEXT("AnimBlueprint'/Game/Blueprints/Animations/ShooterAnim_BP.ShooterAnim_BP'"));

	//if (AnimContainer.Succeeded())
	//	GetMesh()->SetAnimInstanceClass(AnimContainer.Object->GeneratedClass);

	// Create Weapon
	Weapon = CreateDefaultSubobject<UWeaponComponent>("Rifle");

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshContainer(TEXT("SkeletalMesh'/Game/Weapons/Rifle.Rifle'"));
	if (MeshContainer.Succeeded())
		Weapon->SetSkeletalMesh(MeshContainer.Object);

	Weapon->SetRelativeLocation(FVector(1.0f, 4.0f, -2.0f));
	Weapon->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	Weapon->SetupAttachment(GetMesh(), "hand_r");

	// Create Camera
	Camera = CreateDefaultSubobject<UPlayerCameraComponent>("PlayerCamera");
	Camera->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}

EShooterCharacterState AShooterCharacter::GetState() const
{
	return State;
}

void AShooterCharacter::SetState(EShooterCharacterState InState)
{
	PrevState = State;
	State = InState;
}

UWeaponComponent* AShooterCharacter::GetWeaponComponent()
{
	return Weapon;
}

UPlayerCameraComponent* AShooterCharacter::GetCameraComponent()
{
	return Camera;
}

void AShooterCharacter::UpdateAimOffsets(float Pitch, float Yaw)
{
	AimPitch = Pitch;
	AimYaw = Yaw;
}

void AShooterCharacter::InitPlayer()
{
	const FPlayerInfo& PlayerInfo = static_cast<UPlayerGI*>(GetGameInstance())->GetUserInfo();

	InitTeamColor(static_cast<ETeam>(PlayerInfo.TeamNum));
}

void AShooterCharacter::InitTeamColor(ETeam InTeam)
{
	SetTeam(InTeam);
	OnTeamSwitch.Broadcast();
}

void AShooterCharacter::Invincibility(float Duration)
{
	Health = 100000;
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]() { Health = MaxHealth; }, Duration, false);

	InvincibilityFX(Duration);
}

void AShooterCharacter::BeginPlay()
{
	OnTeamSwitch.AddLambda([this]() { RefreshTeamHUD(Team); });
	
	Super::BeginPlay();
	
	RunSpeed = GetCharacterMovement()->MaxWalkSpeed;

	//Invincibility(Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->InvincibilityTime);
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (IsDead())
        return;

	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		if (bIsShooting && !Weapon->Shot())
			Server_StartReload();
		// Anim aim offsets
		FRotator LookRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
		AimPitch = UKismetMathLibrary::ClampAngle(LookRotation.Pitch, -90.f, 90.f);
		AimYaw = UKismetMathLibrary::ClampAngle(LookRotation.Yaw, -90.f, 90.f); // May use later for smooth rotation

		Camera->ShakeCamera(uint8(State), GetLastMovementInputVector().Size());
	}
}

void AShooterCharacter::StartSprint()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_StartSprint();
	}
}

void AShooterCharacter::EndSprint()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_EndSprint();
	}
}

void AShooterCharacter::StartJump()
{
	if (GetNetMode() == NM_Client)
		Server_StartJump();
}

void AShooterCharacter::EndJump()
{
	if (State != EShooterCharacterState::Jump && State != EShooterCharacterState::Falling || GetNetMode() != NM_DedicatedServer)
		return;

	SetState(PrevState);
	Multicast_StopJump();
}

void AShooterCharacter::StartAim()
{
	if (State != EShooterCharacterState::IdleRun)
		return;

	Camera->SwitchToAimCamera();

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_StartAim();
	}
}

void AShooterCharacter::EndAim()
{
	if (State != EShooterCharacterState::Aim)
		return;

	Camera->SwitchToWalkCamera();

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_EndAim();
	}
}

void AShooterCharacter::StartShoot()
{
	if (GetLocalRole() == ROLE_AutonomousProxy && (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Aim))
	{
		Server_StartShoot();
	}
}

void AShooterCharacter::EndShoot()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_EndShoot();
	}
}

void AShooterCharacter::StartReload()
{	
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_StartReload();;
	}
}

void AShooterCharacter::EndReload()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_EndReload();
	}
}
void AShooterCharacter::AbortReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);

	if(GetNetMode() == NM_DedicatedServer)
		Multicast_SetWalkSpeed(RunSpeed);
}

void AShooterCharacter::Falling()
{
	Super::Falling();

	if (State == EShooterCharacterState::Jump)
		return;

	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	SetState(EShooterCharacterState::Falling);
}

void AShooterCharacter::PushButton()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::PushButton);

	ENetRole role = GetLocalRole();

	if (role == ROLE_AutonomousProxy)
	{
		Server_PushButton();
		PlayPushButtonAnim();
	}
	else if (role == ROLE_Authority)
	{
		Multicast_PushButton();
	}

}

void AShooterCharacter::InflictPushButton()
{
	if (GetNetMode() != NM_DedicatedServer)
		return;

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, TSubclassOf<AEnemySpawnerButton>());

	if (OverlappingActors.Num() > 0)
	{
		AEnemySpawnerButton* Button = Cast<AEnemySpawnerButton>(OverlappingActors[0]);
		
		if (Button)
			Button->Activate(Team);
	}
}

void AShooterCharacter::PlayPushButtonAnim()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPushButtonMontage();
}

void AShooterCharacter::Punch()
{
	ENetRole role = GetLocalRole();

	if (role == ROLE_AutonomousProxy)
	{
		Server_Punch();
	}
	else if (role == ROLE_Authority)
	{
		Multicast_Punch();
	}
}

void AShooterCharacter::PlayPunchAnim()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPunchMontage();
}

void AShooterCharacter::StartDisapear()
{
	Super::StartDisapear();
	
	FTimerHandle Handle1;
	GetWorld()->GetTimerManager().SetTimer(Handle1, [this]() { Weapon->SetVisibility(false, true); }, 3.5f, false);

	if (Controller)
	{
		APlayerController* PlayerControler = Cast<APlayerController>(Controller);
		PlayerControler->DisableInput(PlayerControler);
		
		FTimerHandle Handle2;
		GetWorld()->GetTimerManager().SetTimer(Handle2, [PlayerControler]() { PlayerControler->EnableInput(PlayerControler); }, 5.0f, false);
	}
}

void AShooterCharacter::FinishDisapear()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	Super::FinishDisapear();

    if (IsNetMode(NM_DedicatedServer))
    {
	    Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->Respawn(PlayerController);
    }
}


void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, State);
	DOREPLIFETIME(AShooterCharacter, PrevState);
	DOREPLIFETIME(AShooterCharacter, bIsShooting);
	DOREPLIFETIME(AShooterCharacter, AimPitch);
	DOREPLIFETIME(AShooterCharacter, AimYaw);
}

void AShooterCharacter::Server_Punch_Implementation()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::Punch);
	Punch();
}

void AShooterCharacter::Multicast_Punch_Implementation()
{
	PlayPunchAnim();
}

void AShooterCharacter::Server_StartAim_Implementation()
{
	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::Aim);

	Multicast_SetWalkSpeed(AimWalkSpeed);

	Camera->SwitchToAimCamera();
}

void AShooterCharacter::Server_EndAim_Implementation()
{
	if (State != EShooterCharacterState::Aim)
		return;

	SetState(PrevState);

	Multicast_SetWalkSpeed(RunSpeed);

	Camera->SwitchToWalkCamera();
}

void AShooterCharacter::Server_StartShoot_Implementation()
{
	bIsShooting = true;
}

void AShooterCharacter::Server_EndShoot_Implementation()
{
	bIsShooting = false;
}

void AShooterCharacter::Server_StartReload_Implementation()
{
	if (Weapon && Weapon->AmmoCount > 0 && Weapon->WeaponMagazineSize > Weapon->LoadedAmmo)
	{
		if (State == EShooterCharacterState::Aim)
			EndAim();
		else if (bIsShooting)
			bIsShooting = false;

		if (State != EShooterCharacterState::IdleRun)
			return;
		
		SetState(EShooterCharacterState::Reload);

		Multicast_SetWalkSpeed(ReloadWalkSpeed);
	}
}

void AShooterCharacter::Server_EndReload_Implementation()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);

	Multicast_SetWalkSpeed(RunSpeed);

	if (Weapon)
		Weapon->Reload();
}

void AShooterCharacter::Server_StartSprint_Implementation()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Reload)
		AbortReload();
	else if (State == EShooterCharacterState::Aim)
		EndAim();

	if (State != EShooterCharacterState::IdleRun && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::Sprint;
	else
		SetState(EShooterCharacterState::Sprint);

	if (State == EShooterCharacterState::Reload)
		AbortReload();

	else if (State == EShooterCharacterState::Aim)
		EndAim();

	Multicast_SetWalkSpeed(SprintSpeed);
}

void AShooterCharacter::Server_EndSprint_Implementation()
{
	if (State != EShooterCharacterState::Sprint && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::IdleRun;
	else
		SetState(EShooterCharacterState::IdleRun);

	Multicast_SetWalkSpeed(RunSpeed);
}

void AShooterCharacter::Server_PushButton_Implementation()
{
	PushButton();
}

void AShooterCharacter::Multicast_PushButton_Implementation()
{
	PlayPushButtonAnim();
}

void AShooterCharacter::Multicast_SetWalkSpeed_Implementation(float WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AShooterCharacter::Multicast_Jump_Implementation()
{
	Jump();
}

void AShooterCharacter::Server_StartJump_Implementation()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (CanJump() && (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Sprint))
	{
		SetState(EShooterCharacterState::Jump);
		Multicast_Jump();
	}
}

void AShooterCharacter::Multicast_StopJump_Implementation()
{
	StopJumping();
}