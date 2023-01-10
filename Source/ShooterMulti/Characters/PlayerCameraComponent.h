#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "MatineeCameraShake.h"
#include "Camera/CameraComponent.h"
#include "PlayerCameraComponent.generated.h"

UCLASS()
class SHOOTERMULTI_API UPlayerCameraComponent : public USpringArmComponent
{
	GENERATED_BODY()

private:

	float TargetSpringArmLength = 0.0f;
	float TargetCameraFOV = 0.0f;
	float CurrentTime = 0.0f;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Camera", meta = (ClampMin = "0.0"))
	float LerpSpeed = 6.4f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Camera", meta = (ClampMin = "0.0"))
	float CameraAimDistance = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Camera", meta = (ClampMin = "0.0"))
	float CameraAimFOV = 75.f;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	float CameraResetDistance;

	UPROPERTY(Transient, BlueprintReadOnly)
	float CameraResetFOV;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Camera")
	TSubclassOf<UMatineeCameraShake> RunShake;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Camera")
	TSubclassOf<UMatineeCameraShake> SprintShake;

	UPROPERTY(Category = "Character|Camera", VisibleDefaultsOnly, BlueprintReadOnly)
	class UCameraComponent* Camera;

public:
	UPlayerCameraComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Character|Camera")
	void SwitchToAimCamera();
	UFUNCTION(BlueprintCallable, Category = "Character|Camera")
	void SwitchToWalkCamera();
	UFUNCTION(BlueprintCallable, Category = "Character|Camera")
	void ShakeCamera(uint8 State, float Intensity);
	UFUNCTION(BlueprintCallable, Category = "Character|Camera")
	UCameraComponent* GetCameraHandle() const;
};
