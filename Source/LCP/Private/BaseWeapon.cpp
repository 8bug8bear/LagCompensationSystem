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

	DOREPLIFETIME_CONDITION(ABaseWeapon, bWaitingReload, COND_SkipOwner);
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
	Reload();
}

bool ABaseWeapon::ServerStartReload_Validate()
{
	return true;
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

