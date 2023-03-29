// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;


	AmountAmmo = 0;
	AmmoInMagazine = 0;

	WeaponState = EWeaponState::Idle;
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseWeapon, AmountAmmo,     COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABaseWeapon, AmmoInMagazine, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ABaseWeapon, bWaitingReload, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABaseWeapon, ShotCounter,    COND_SkipOwner);
}

void ABaseWeapon::PostInitializeComponents()
{
	AmmoInMagazine = MagazineSize;
	AmountAmmo = TotalAmmo - MagazineSize;
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

}

void ABaseWeapon::StartFire()
{

}

void ABaseWeapon::StopFire()
{

}

void ABaseWeapon::Reload()
{

}

bool ABaseWeapon::CanFire() const
{
	return false;
}

bool ABaseWeapon::CanReload() const
{
	return false;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

