#include "WeaponComponent.h"
#include "BeamLight.h"
#include "../Characters/ShooterCharacter.h"
#include "../Controllers/ShooterController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Particles/ParticleSystemComponent.h"

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	LightPool.BeginPlay(GetWorld(), 4u);
	LoadedAmmo = WeaponMagazineSize;
	AmmoCount = MaxAmmo < AmmoCount? MaxAmmo : AmmoCount;
	CurrentSpread = 0.f;

	SetIsReplicated(true);
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	ShootTimer += DeltaTime;

	//update spread
	AShooterCharacter* Character = static_cast<AShooterCharacter*>(GetOwner());
	float MinSpread = (Character->GetState() == EShooterCharacterState::Aim) ? WeaponMinSpreadAim : WeaponMinSpreadWalk;
	CurrentSpread = FMath::Max(MinSpread, CurrentSpread - WeaponSpreadRecoveryRate * DeltaTime);

	if(GetNetMode() == NM_Client)
		Server_Tick(GetSocketTransform("MuzzleFlashSocket"));
}

bool UWeaponComponent::Shot()
{
	if (ShootTimer < FireRate)
		return true;

	ShootTimer = 0.f;

	if (LoadedAmmo <= 0)
		return false;

	--LoadedAmmo;

	FLaserWeaponData WeaponData;
	WeaponData.MuzzleTransform = MuzzleFlashTransform;
	WeaponData.LookTransform = Cast<AShooterCharacter>(GetOwner())->GetCameraComponent()->GetCameraHandle()->GetComponentTransform();
	WeaponData.Damages = WeaponDamage;
	WeaponData.Knockback = WeaponKnockback;
	WeaponData.Spread = CurrentSpread;
	FHitResult HitResult;
	bool HasHit = ShootLaser(GetOwner(), HitResult, WeaponData);

	//add spread
	CurrentSpread = FMath::Min(WeaponMaxSpread, CurrentSpread + WeaponSpreadPerShot);
	
	Multicast_Shot(WeaponData, HitResult.Actor.Get(), HitResult.ImpactPoint, HitResult.ImpactNormal, HitResult.Normal, HasHit);
	
	return true;
}

void UWeaponComponent::Reload()
{
	if (AmmoCount <= 0)
		return;

	int temp = WeaponMagazineSize - LoadedAmmo;
	temp = temp > AmmoCount ? AmmoCount : temp;
	LoadedAmmo += temp;
	AmmoCount -= temp;
}

void UWeaponComponent::GetAmmo(int Count)
{
	AmmoCount = FMath::Min(AmmoCount + Count, MaxAmmo);
}

// Weapon Utiliy

bool UWeaponComponent::ShootLaser(AActor* Causer, FHitResult& HitResult, const FLaserWeaponData& WeaponData)
{
	ENetMode netMode = GetNetMode();

	FVector LookLocation = WeaponData.LookTransform.GetLocation();
	FVector LookDirection = WeaponData.LookTransform.GetRotation().GetForwardVector();

	//apply spread
	if (WeaponData.Spread > 0.f)
		LookDirection = UKismetMathLibrary::RandomUnitVectorInConeInRadians(LookDirection,
																			FMath::DegreesToRadians(WeaponData.Spread * .5f));

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Causer);
	CollisionParams.bTraceComplex = true;
	CollisionParams.bReturnPhysicalMaterial = true;

	//in case of actor hit
	if (GetWorld()->LineTraceSingleByChannel(	HitResult,
												LookLocation,
												LookLocation + LookDirection * WeaponData.MaxDistance,
												ECC_Visibility , CollisionParams))
	{
		//make damages
		FPointDamageEvent DamageEvent = FPointDamageEvent(WeaponData.Damages, HitResult, LookDirection, UDamageType::StaticClass());
		HitResult.Actor->TakeDamage(WeaponData.Damages, DamageEvent, nullptr, Causer);

		//push hit actors (physics)
		TArray<UActorComponent*> SkeletalMeshComponents;
		HitResult.Actor->GetComponents(USkeletalMeshComponent::StaticClass(), SkeletalMeshComponents);
		for (auto ActorComponent : SkeletalMeshComponents)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ActorComponent);
			if (SkeletalMeshComponent->IsSimulatingPhysics())
				SkeletalMeshComponent->AddForceAtLocation(LookDirection * WeaponData.Knockback, HitResult.ImpactPoint, HitResult.BoneName);
		}
		TArray<UActorComponent*> StaticMeshComponents;
		HitResult.Actor->GetComponents(UStaticMeshComponent::StaticClass(), StaticMeshComponents);
		for (auto ActorComponent : StaticMeshComponents)
		{
			UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponent);
			if (StaticMeshComponent->IsSimulatingPhysics())
				StaticMeshComponent->AddForceAtLocation(LookDirection * WeaponData.Knockback, HitResult.ImpactPoint, HitResult.BoneName);
		}

		return Cast<ACharacter>(HitResult.Actor) == nullptr; // if collision with non character.
	}
	//when no actor hit
	else
	{
		HitResult.ImpactPoint = LookLocation + LookDirection * WeaponData.MaxDistance;
		HitResult.Distance = WeaponData.MaxDistance;
		return false;
	}
}

void UWeaponComponent::MakeImpactDecal(	const AActor* actor,
										const FVector& ImpactPoint,
										const FVector& ImpactNormal,
										UMaterialInterface* ImpactDecalMaterial,
										float ImpactDecalSizeMin,
										float ImpactDecalSizeMax)
{
	auto StaticMeshComponent = actor->FindComponentByClass<UStaticMeshComponent>();
	if (StaticMeshComponent)
	{
		FVector DecalPos = ImpactPoint;
		FRotator DecalRot = (ImpactNormal.Rotation().Quaternion() * FRotator(0.f, 0.f, FMath::RandRange(-180.f, 180.f)).Quaternion()).Rotator();
		float RandomSize = FMath::RandRange(ImpactDecalSizeMin, ImpactDecalSizeMax);
		FVector DecalSize = FVector(RandomSize, RandomSize, RandomSize);

		if (StaticMeshComponent->Mobility == EComponentMobility::Static)
		{
			UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), ImpactDecalMaterial, DecalSize,
				DecalPos, DecalRot, 0.f);
			if (DecalComponent)
				DecalComponent->FadeScreenSize = 11.f;
		}
		else
		{
			UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAttached(ImpactDecalMaterial, DecalSize, StaticMeshComponent,
				NAME_None, DecalPos, DecalRot, EAttachLocation::KeepWorldPosition, 0.f);
			if (DecalComponent)
				DecalComponent->FadeScreenSize = 11.f;
		}
	}
}

void UWeaponComponent::MakeLaserBeam(	FVector Start,
										FVector End,
										UParticleSystem* BeamParticles,
										float InBeamIntensity,
										FLinearColor Color,
										UCurveFloat* InBeamIntensityCurve)
{
	FTransform ParticleTransform;
	ParticleTransform.SetLocation(Start);
	ParticleTransform.SetRotation((End - Start).Rotation().Quaternion());

	//create a beam particle
	SpawnEmitterAtLocation(BeamParticles, ParticleTransform, Start, End);

	LightPool.Spawn(0.8f)->Initialize(Start, End, Color, 0.8f, InBeamIntensity, InBeamIntensityCurve);
}

void UWeaponComponent::MakeImpactParticles( UParticleSystem* ImpactParticles,
										    const FVector& ImpactPoint,
										    const FVector& Normal,
											float Scale)
{
	FTransform HitTransform;
	HitTransform.SetLocation(ImpactPoint);
	HitTransform.SetRotation(Normal.Rotation().Quaternion());
	HitTransform.SetScale3D(FVector(Scale, Scale, Scale));

	SpawnEmitterAtLocation(ImpactParticles, HitTransform);
}

void UWeaponComponent::SpawnEmitterAtLocation(	UParticleSystem* EmitterTemplate,
												const FTransform& SpawnTransform,
												const FVector& Source,
												const FVector& Target)
{
	if (EmitterTemplate && GetNetMode() != NM_DedicatedServer)
	{
		UParticleSystemComponent* ParticleSystemComponent = UGameplayStatics::SpawnEmitterAtLocation(	GetWorld(),
																					EmitterTemplate,
																					SpawnTransform,
																					true,
																					EPSCPoolMethod::AutoRelease);
		if (Source != FVector::ZeroVector && Target != FVector::ZeroVector)
		{
			ParticleSystemComponent->SetBeamSourcePoint(0, Source, 0);
			ParticleSystemComponent->SetBeamTargetPoint(0, Target, 0);
		}
	}
}


void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, AmmoCount);
	DOREPLIFETIME(UWeaponComponent, LoadedAmmo);
	DOREPLIFETIME(UWeaponComponent, MuzzleFlashTransform);
}

void UWeaponComponent::Multicast_Shot_Implementation(const FLaserWeaponData& WeaponData,
													 const AActor* Actor,
													 const FVector& ImpactPoint,
													 const FVector& ImpactNormal,
													 const FVector& Normal,
													 bool HasHit)
{
	if (GetNetMode() == NM_DedicatedServer)
		return;

	if (HasHit)
	{
		//make impact decal
		MakeImpactDecal(Actor, ImpactPoint, ImpactNormal, ImpactDecalMat, .9f * ImpactDecalSize, 1.1f * ImpactDecalSize);

		//create impact particles
		MakeImpactParticles(ImpactParticle, ImpactPoint, Normal, .66f);
	}

	//make the beam visuals
	MakeLaserBeam(WeaponData.MuzzleTransform.GetLocation(), ImpactPoint, BeamParticle, BeamIntensity, FLinearColor(1.f, 0.857892f, 0.036923f), BeamIntensityCurve);

	//play the shot sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotSound, WeaponData.MuzzleTransform.GetLocation());

	//make muzzle smoke
	UGameplayStatics::SpawnEmitterAttached(MuzzleSmokeParticle, this, FName("MuzzleFlashSocket"));

	//apply shake
	auto PlayerController = Cast<AShooterController>(Cast<AShooterCharacter>(GetOwner())->GetController());
	if (PlayerController && ShootShake)
		PlayerController->ClientPlayCameraShake(ShootShake);

	if (LoadedAmmo == 0)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotEmptySound, GetOwner()->GetActorLocation());
}

void UWeaponComponent::Server_Tick_Implementation(const FTransform& Transform)
{
	MuzzleFlashTransform = Transform;
}