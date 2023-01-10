#pragma once

#include "HealthCharacter.h"
#include "../Weapons/WeaponComponent.h"
#include "PlayerCameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class EShooterCharacterState : uint8
{
	IdleRun,
	Aim,
	Sprint,
	Reload,
	Jump,
	Falling,
	Punch,
	Dead,
	PushButton
};

UCLASS()
class SHOOTERMULTI_API AShooterCharacter : public AHealthCharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	UWeaponComponent* Weapon;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	UPlayerCameraComponent* Camera;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character|Shooter")
	EShooterCharacterState State;
	UPROPERTY(Replicated)
	EShooterCharacterState PrevState;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float AimPitch;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float AimYaw;

	void UpdateAimOffsets(float Pitch, float Yaw);

	void PlayPushButtonAnim();

	void PlayPunchAnim();

	void Falling() override;

	void BeginPlay() override;

	void Invincibility(float Duration);

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Shooter")
	void InvincibilityFX(float Duration);
	void InvincibilityFX_Implementation(float Duration) {};

public:
	UPROPERTY(Replicated)
	bool bIsShooting = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float SprintSpeed = 1000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float AimWalkSpeed = 180.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float ReloadWalkSpeed = 200.f;

	UPROPERTY(BlueprintReadOnly)
	float RunSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MinSprintMagnitude = .3f;

	AShooterCharacter();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	EShooterCharacterState GetState() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void SetState(EShooterCharacterState InState);

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	UWeaponComponent* GetWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	UPlayerCameraComponent* GetCameraComponent();

	void InitPlayer();

	void InitTeamColor(ETeam InTeam);

	void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartSprint();
	UFUNCTION()
	void EndSprint();

	UFUNCTION()
	void StartJump();
	UFUNCTION()
	void EndJump();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartAim();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndAim();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartShoot();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndShoot();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartReload();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndReload();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void AbortReload();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void PushButton();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void InflictPushButton();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void Punch();

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Shooter")
	void RefreshTeamHUD(ETeam InTeam);
	void RefreshTeamHUD_Implementation(ETeam InTeam) {};

	void StartDisapear() override;
	void FinishDisapear() override;

private:
#pragma region Server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//Punch
	UFUNCTION(Server, Reliable)
	void Server_Punch();
	void Server_Punch_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Punch();
	void Multicast_Punch_Implementation();

	//Start aim
	UFUNCTION(Server, Reliable)
	void Server_StartAim();
	void Server_StartAim_Implementation();

	//End aim
	UFUNCTION(Server, Reliable)
	void Server_EndAim();
	void Server_EndAim_Implementation();

	//Start Shooting
	UFUNCTION(Server, Reliable)
	void Server_StartShoot();
	void Server_StartShoot_Implementation();

	//Start Shooting
	UFUNCTION(Server, Reliable)
	void Server_EndShoot();
	void Server_EndShoot_Implementation();

	//Start reload
	UFUNCTION(Server, Reliable)
	void Server_StartReload();
	void Server_StartReload_Implementation();

	//End reload
	UFUNCTION(Server, Reliable)
	void Server_EndReload();
	void Server_EndReload_Implementation();

	//Start sprint
	UFUNCTION(Server, Reliable)
	void Server_StartSprint();
	void Server_StartSprint_Implementation();

	//End sprint
	UFUNCTION(Server, Reliable)
	void Server_EndSprint();
	void Server_EndSprint_Implementation();
	
	//Press button
	UFUNCTION(Server, Reliable)
	void Server_PushButton();
	void Server_PushButton_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_StartJump();
	void Server_StartJump_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PushButton();
	void Multicast_PushButton_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetWalkSpeed(float WalkSpeed);
	void Multicast_SetWalkSpeed_Implementation(float WalkSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Jump();
	void Multicast_Jump_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopJump();
	void Multicast_StopJump_Implementation();
#pragma endregion
};