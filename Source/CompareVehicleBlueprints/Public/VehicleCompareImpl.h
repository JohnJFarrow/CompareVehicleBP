// Copyright John Farrow (c) 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnimationGraph.h"

#include "Difference.h"
#include "VehicleCompareImpl.generated.h"

/**
 * compare vehicle blueprints 
 */
UCLASS()
class COMPAREVEHICLEBLUEPRINTS_API UVehicleCompareImpl: public UObject
{
	GENERATED_BODY()

public:
	void CompareVehicleBlueprints(const FString& VehicleAssetPath1, const FString& VehicleAssetPath2);

	const TArray<TSharedRef< class FDifference >>& GetResults() const;

private:
	void CompareVehicleMovementComponents( const FString& PathA, const FString& PathB, const class UChaosWheeledVehicleMovementComponent* A, const UChaosWheeledVehicleMovementComponent* B);
	void CompareSkeletalMeshComponents( const FString& PathA, const FString& PathB, const class USkeletalMeshComponent* A, const USkeletalMeshComponent* B);

	// output messages
	void AddMessage(const FString& Message, const EDifferenceType& Type );
	void AddWarning(const FString& Message);
	void AddError(const FString& Message);
	void AddInfo(const FString& Message);
	void Report(FString PathA, FString PathB, const FString& Type, const FProperty* Property, const FString& StringValueA, const FString& StringValueB);

	// compare types of properties
	void CompareProperty(const FString& PathA, const FString& PathB, FProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FEnumProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FBoolProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FNumericProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FStrProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FClassProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FTextProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FNameProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FObjectPtrProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FSoftObjectProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);
	void Compare(const FString& PathA, const FString& PathB, FStructProperty* StructProperty, const uint8* StructAddrA, const uint8* StructAddrB);
	void Compare(const FString& PathA, const FString& PathB, FArrayProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB);


private:
	TArray<FString> GetAllPropertyNames(UClass* Class);

	// log differences
	TArray<TSharedRef<FDifference>> Results;
};
