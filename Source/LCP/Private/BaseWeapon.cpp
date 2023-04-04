// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Net/UnrealNetwork.h"
#include "LCPCharacter.h"
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
		// ������������� ��������
		break;
	case EWeaponState::Firing:
		HandleFiring();
		break;
	case EWeaponState::Reloading:
		// ������������� ��������, �������� �����������
		break;
	case EWeaponState::Equipping:
		// ������������� ��������, ������������� �����������
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

void ABaseWeapon::HandleFiring()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Shot))
	{
		return;
	}

	float InFireDelay = 60.f / RateOfFire;

	GetWorldTimerManager().SetTimer(TimerHandle_Shot, this, &ABaseWeapon::Shot, InFireDelay, true);
}

void ABaseWeapon::Shot()
{
	if (!bWontsFire || (!bIsAutomaticWeapon && ShotCounter > 0))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Shot);
		return;
	}

	if (AmmoInMagazine > 0)
	{
		
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

