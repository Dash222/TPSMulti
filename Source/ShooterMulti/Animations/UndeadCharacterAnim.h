
#pragma once

#include "Animation/AnimInstance.h"
#include "../Characters/UndeadCharacter.h"
#include "UndeadCharacterAnim.generated.h"

UCLASS()
class SHOOTERMULTI_API UUndeadCharacterAnim : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation|Shooter")
	AUndeadCharacter* UndeadCharacter = nullptr;

public:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation|Shooter")
	FVector2D Direction;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation|Shooter")
	float Speed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation|Shooter")
	UAnimMontage* PunchMontage;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UFUNCTION()
	void AnimNotify_PunchHit(UAnimNotify* Notify) const;

	void PlayPunchMontage();
};
