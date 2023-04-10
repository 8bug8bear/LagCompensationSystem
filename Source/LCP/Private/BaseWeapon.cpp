// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "LCP.h"
#include "Net/UnrealNetwork.h"
#include "LCPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
{
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bOnlyOwnerSee = true;
	RootComponent = Mesh1P;

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P"));
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh1P->bOwnerNoSee = true;
	Mesh1P->bOnlyOwnerSee = false;
	Mesh1P->SetupAttachment(Mesh1P);

	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	PrimaryActorTick.bCanEverTick = true;

	AmountAmmo = 0;
	AmmoInMagazine = 0;

	WeaponState = EWeaponState::Idle;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, OwningPlayer);

	DOREPLIFETIME_CONDITION(ABaseWeapon, AmountAmmo,     COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABaseWeapon, AmmoInMagazine, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ABaseWeapon, IsReloading,    COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABaseWeapon, ShotCounter,    COND_SkipOwner);
}

void ABaseWeapon::PostInitializeComponents()
{
	AmmoInMagazine = MagazineSize;
	AmountAmmo = TotalAmmo - MagazineSize;
}

void ABaseWeapon::SetOwningPlayer(ALCPCharacter* NewOwningPlayer)
{
	if (OwningPlayer != NewOwningPlayer)
	{
		OwningPlayer = NewOwningPlayer;
		SetOwner(OwningPlayer);
	}
}

void ABaseWeapon::AttachWeapon()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerAttachWeapon();
	}


}

void ABaseWeapon::DettachWeapon()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerDettachWeapon();
	}
}

void ABaseWeapon::ServerAttachWeapon_Implementation()
{
	AttachWeapon();
}

bool ABaseWeapon::ServerAttachWeapon_Validate()
{
	return true;
}

void ABaseWeapon::ServerDettachWeapon_Implementation()
{
	DettachWeapon();
}

bool ABaseWeapon::ServerDettachWeapon_Validate()
{
	return true;
}

void ABaseWeapon::OnRep_Reload()
{
	if (bWaitingReload)
	{

	}
	else
	{

	}
}

void ABaseWeapon::OnRep_ShotCounter()
{
	if (ShotCounter > 0)
	{
		SimulateShot();
	}
}

void ABaseWeapon::StartFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWontsFire)
	{
		bWontsFire = true;
		DetermineWeaponState();
	}
}

void ABaseWeapon::StopFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bWontsFire)
	{
		bWontsFire = false;
		DetermineWeaponState();
	}
}

void ABaseWeapon::Reload(bool bFromClient)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartReload();
	}

	if (!bWaitingReload)
	{
		bWaitingReload = true;
		DetermineWeaponState();
	}
}

void ABaseWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool ABaseWeapon::ServerStartFire_Validate()
{
	return true;
}

void ABaseWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool ABaseWeapon::ServerStopFire_Validate()
{
	return true;
}

void ABaseWeapon::ServerStartReload_Implementation()
{
	Reload(true);
}

bool ABaseWeapon::ServerStartReload_Validate()
{
	return true;
}


void ABaseWeapon::ClientStartReload_Implementation()
{
	Reload(false);
}

void ABaseWeapon::SetWeaponState(EWeaponState NewState)
{
	switch (NewState)
	{
	case EWeaponState::Idle:
		// остонавливаем стрельбу
		break;
	case EWeaponState::Firing:
		OnBurstStarted();
		break;
	case EWeaponState::Reloading:
		// Останавливаем стрельбу, начинаем перезарядку
		break;
	case EWeaponState::Equipping:
		// Останавливаем стрельбу, останавливаем перезарядку
		break;
	default:
		
		break;
	}
}

void ABaseWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bWaitingReload && CanReload())
		{
			NewState = EWeaponState::Reloading;
		}
		else if(bWontsFire && CanFire())
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if(bWaitingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}

void ABaseWeapon::OnBurstStarted()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Shot))
	{
		return;
	}

	float InFireDelay = 60.f / RateOfFire;

	GetWorldTimerManager().SetTimer(TimerHandle_Shot, this, &ABaseWeapon::Shot, FMath::Max<float>(InFireDelay, SMALL_NUMBER), true);
}

void ABaseWeapon::OnBurstFinished()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Shot);

	ShotCounter = 0;
}

void ABaseWeapon::WeaponTrace()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float ConeHalfAngle = FMath::DegreesToRadians(Spread * 0.5f);

	FVector CameraLocation = FVector::ZeroVector;
	FRotator CameraRotation = FRotator::ZeroRotator;

	if (OwningPlayer)
	{
		AController* Controller = OwningPlayer->GetController();
		if (Controller)
		{
			Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);
		}
	}

	const FVector TraceDirection = WeaponRandomStream.VRandCone(CameraRotation.Vector(), ConeHalfAngle, ConeHalfAngle);
	const FVector TraceEnd = CameraLocation + TraceDirection * BIG_NUMBER;

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, OwningPlayer);

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, COLLISION_WEAPON, TraceParams);

	if (Hit.GetActor())
	{
		UGameplayStatics::ApplyDamage(Hit.GetActor(), Damage, OwningPlayer->GetController(), this, UDamageType::StaticClass());
	}
}

void ABaseWeapon::Shot()
{
	if (!bWontsFire || (!bIsAutomaticWeapon && ShotCounter > 0))
	{
		OnBurstFinished();
		return;
	}

	if (AmmoInMagazine > 0)
	{
		ShotCounter++;

		if (GetLocalRole() < ROLE_Authority)
		{
			SimulateShot();
		}
		else
		{
			AmmoInMagazine--;

			for (int i = 0; i < BulletInAmmo; i++)
			{
				WeaponTrace();
			}
		}
	}
	else
	{
		OnBurstFinished();

		if (CanReload())
		{

		}
	}
}

void ABaseWeapon::SimulateShot()
{

}

bool ABaseWeapon::CanFire() const
{
	return (WeaponState == EWeaponState::Idle && OwningPlayer && !bWaitingReload);
}

bool ABaseWeapon::CanReload() const
{
	bool bStateAllowsReload = ((WeaponState == EWeaponState::Idle) || (WeaponState == EWeaponState::Firing));
	bool bGotAmmo = ((AmmoInMagazine < MagazineSize) && (AmountAmmo > 0));
	return (bStateAllowsReload && bGotAmmo && OwningPlayer);
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

