// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UENUM()
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Reloading,
	Equipping,
};

UCLASS()
class LCP_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	

private:
	UPROPERTY(ReplicatedUsing = OnRep_Reload)
	bool bWaitingReload = false;

	bool bWontsFire = false;

	UPROPERTY(ReplicatedUsing = OnRep_ShotCounter)
	int32 ShotCounter = 0;

protected:
	/*The maximum number of ammunition that can be carried with this weapon*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = 1))
	int32 TotalAmmo = 150;

	/*The maximum amount of Ammo placed in the magazine*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = 1))
	int32 MagazineSize = 30;

	/*The total amount of ammo that this weapon has at the moment*/
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Ammo")
	int32 AmountAmmo;

	/*The total amount of ammo in the magazine*/
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Ammo")
	int32 AmmoInMagazine;

	EWeaponState WeaponState;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ABaseWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** perform initial setup */
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnRep_Reload();

	UFUNCTION()
	void OnRep_ShotCounter();


	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Inpet
	virtual void StartFire();

	virtual void StopFire();

	virtual void Reload();


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
