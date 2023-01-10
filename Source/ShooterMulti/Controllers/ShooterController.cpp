#include "ShooterController.h"
#include "../Characters/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void AShooterController::BeginPlayingState()
{
	Super::BeginPlayingState();

	ShooterCharacter = Cast<AShooterCharacter>(GetPawn());

    if (const AShooterPS* ShooterPS = Cast<AShooterPS>(PlayerState))
    {
        ShooterCharacter->InitTeamColor(static_cast<ETeam>(ShooterPS->TeamNum));
    }
}

void AShooterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AShooterController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AShooterController::MoveRight);
	InputComponent->BindAxis("LookUp", this, &AShooterController::LookUp);
	InputComponent->BindAxis("Turn", this, &AShooterController::Turn);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &AShooterController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AShooterController::EndSprint);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AShooterController::StartAim);
	InputComponent->BindAction("Aim", IE_Released, this, &AShooterController::EndAim);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AShooterController::StartReload);
	InputComponent->BindAction("Punch", IE_Pressed, this, &AShooterController::Punch);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AShooterController::StartJump);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShooterController::StartShoot);
	InputComponent->BindAction("Shoot", IE_Released, this, &AShooterController::EndShoot);
	InputComponent->BindAction("PushButton", IE_Pressed, this, &AShooterController::PushButton);
}

void AShooterController::MoveForward(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		if (ShooterCharacter->GetState() == EShooterCharacterState::Sprint && Value <= 0.0f)
			EndSprint();

		FRotator Rotation = GetControlRotation();
		Rotation.Pitch = 0.f;
		Rotation.Roll = 0.f;

		ShooterCharacter->AddMovementInput(Value * Rotation.GetNormalized().Vector());
	}

}
void AShooterController::MoveRight(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead()	&& ShooterCharacter->GetState() != EShooterCharacterState::Sprint
																	&& ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		FRotator Rotation = GetControlRotation();
		Rotation.Pitch = 0.f;
		Rotation.Roll = 0.f;

		ShooterCharacter->AddMovementInput(Value * Rotation.GetNormalized().RotateVector(FVector::RightVector));
	}
}

void AShooterController::LookUp(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		AddPitchInput(Value);
	}
}
void AShooterController::Turn(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		AddYawInput(Value);
	}
}

void AShooterController::StartSprint()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartSprint();
}

void AShooterController::EndSprint()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndSprint();
}

void AShooterController::StartJump()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartJump();
}

void AShooterController::EndJump()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndJump();
}


void AShooterController::StartAim()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartAim();
}

void AShooterController::EndAim()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndAim();
}

void AShooterController::StartShoot()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartShoot();
}

void AShooterController::EndShoot()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndShoot();
}

void AShooterController::StartReload()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartReload();
}

void AShooterController::EndReload()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndReload();
}

void AShooterController::PushButton()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->PushButton();
}

void AShooterController::Punch()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->Punch();
}

void AShooterController::DisableInput(APlayerController* PlayerController)
{
	Super::DisableInput(PlayerController);
	
	EndSprint();
	EndAim();
	EndShoot();
}