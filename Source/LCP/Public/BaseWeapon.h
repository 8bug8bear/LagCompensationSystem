// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class UAnimMontage;
class ALCPCharacter;
class UAudioComponent;
class UParticleSystemComponent;
class USoundCue;

UENUM()
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Reloading,
	Equipping,
};

USTRUCT()
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

		/** animation played on pawn (1st person view) */
		UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Pawn1P;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Pawn3P;

	FWeaponAnim()
	{
		Pawn1P = nullptr;
		Pawn3P = nullptr;
	}
};


UCLASS()
class LCP_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	

private:
	bool bWaitingReload = false;

	UPROPERTY(ReplicatedUsing = OnRep_Reload)
	bool IsReloading = false;

	bool bWaitingEquip = false;

	bool bIsEquipped = false;

	bool bWontsFire = false;

	UPROPERTY(ReplicatedUsing = OnRep_ShotCounter)
	int32 ShotCounter = 0;

protected:
	EWeaponState WeaponState;

	UPROPERTY(Replicated)
	ALCPCharacter* OwningPlayer;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Ammo
	// The maximum number of ammunition that can be carried with this weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = 1))
	int32 TotalAmmo = 150;

	// The maximum amount of Ammo placed in the magazine
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = 1))
	int32 MagazineSize = 30;

	// The total amount of ammo that this weapon has at the moment
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Ammo")
	int32 AmountAmmo;

	// The total amount of ammo in the magazine
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Ammo")
	int32 AmmoInMagazine;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Shooting
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting", meta = (ClampMin = 1.f))
	float Damage = 30;

	// Number of shots per minute
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting", meta = (ClampMin = 1.f))
	float RateOfFire = 600;

	// Determines whether the weapon can fire in bursts or single
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting")
	bool bIsAutomaticWeapon = false;

	// The angle of the spread of weapons in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting")
	float Spread = 5.f;

	// The number of bullets fired in one shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Shooting", meta = (ClampMin = 1))
	int32 BulletInAmmo = 1;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Visual
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh3P;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim EquipAnim;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	FWeaponAnim FireAnima;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Timers
	FTimerHandle TimerHandle_Shot;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ABaseWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Perform initial setup 
	virtual void PostInitializeComponents() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Inventory 
	void SetOwningPlayer(ALCPCharacter* NewOwningPlayer);

	//[local + server]
	void AttachWeapon();

	//[local + server]
	void DettachWeapon();

protected:
	UFUNCTION(reliable, server, WithValidation)
	void ServerAttachWeapon();

	UFUNCTION(reliable, server, WithValidation)
	void ServerDettachWeapon();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Multicast events
	UFUNCTION()
	void OnRep_Reload();

	UFUNCTION()
	void OnRep_ShotCounter();

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Inpet [local + server]
	virtual void StartFire();

	virtual void StopFire();

	virtual void Reload(bool bFromClient);

protected:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Inpet in server
	UFUNCTION(reliable, server, WithValidation)
	void ServerStartFire();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStopFire();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStartReload();

	UFUNCTION(reliable, client)
	void ClientStartReload();
	
	void SetWeaponState(EWeaponState NewState);

	void DetermineWeaponState();

	void OnBurstStarted();

	void OnBurstFinished();

	void WeaponTrace();

	UFUNCTION()
	void Shot();

	void SimulateShot();

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Control
	bool CanFire() const;

	bool CanReload() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Reading data
	UFUNCTION(BlueprintCallable)
	int32 GetAmountAmmo() const { return AmountAmmo; }

	UFUNCTION(BlueprintCallable)
	int32 GetAmmoInMagazine() const { return AmmoInMagazine; }
};
