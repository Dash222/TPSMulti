#pragma once

#include "Components/SceneComponent.h"
#include "InvFXComponent.generated.h"


UCLASS( ClassGroup = Custom, meta = (BlueprintSpawnableComponent) )
class SHOOTERMULTI_API UInvFXComponent : public USceneComponent
{
	GENERATED_BODY()

protected:

	float CurrentTime = 0.0f;

	UStaticMeshComponent* Mesh1 = nullptr;
	UStaticMeshComponent* Mesh2 = nullptr;

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Duration = 3.0f;

	UInvFXComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};