// Copyright John Farrow (c) 2023. All Rights Reserved.


#include "VehicleCompareImpl.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/NoExportTypes.h"
#include "K2Node_EnhancedInputAction.h"
#include "EdGraphSchema_K2_Actions.h"
#include "EditorAssetLibrary.h"
#include "InputAction.h"
#include "UObject\UnrealType.h"
#include "UObject/NoExportTypes.h"
#include "SubobjectDataSubsystem.h"
#include "SubobjectDataBlueprintFunctionLibrary.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "UObject\UnrealTypePrivate.h"
#include "Difference.h"
#include "ReferenceSkeleton.h"

//error C4456 declaration of 'TypedProperty' hides previous local declaration

#pragma warning( disable: 4456 )
#pragma warning( disable: 4457 )

namespace
{
	const FString Quote = "\"";

	FString AppendDisplayName(const FString& Path, const FProperty* Property)
	{
		if (!Property)
		{
			return Path;
		}

		FString DisplayName = Property->GetDisplayNameText().ToString();
		FString Name = Property->GetName();

		if (DisplayName != Name)
		{
			if (DisplayName.Contains(" "))
			{
				DisplayName = "\"" + DisplayName + "\"";
			}

			Name = DisplayName;
		}

		return Path + "/" + Name;
	}


}

void UVehicleCompareImpl::Report(FString PathA, FString PathB, const FString& Type, const FProperty* Property, const FString& StringValueA, const FString& StringValueB)
{
	if (!Property) return;

	PathA = AppendDisplayName(PathA, Property);
	PathB = AppendDisplayName(PathB, Property);

	TSharedRef<FDifference> Diff = MakeShared<FDifference>();
	Diff->Type = EDifferenceType::Difference;
	Diff->Paths.Add(PathA);
	Diff->Paths.Add(PathB);
	Diff->ValuesAsString.Add(StringValueA);
	Diff->ValuesAsString.Add(StringValueB);
	Results.Add(Diff);
}

void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FEnumProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	UEnum* EnumDef = Property->GetEnum();
	FNumericProperty* UnderlyingProperty = Property->GetUnderlyingProperty();
	check(UnderlyingProperty);
	int32 IntValueA = UnderlyingProperty->GetSignedIntPropertyValue(PropertyAddrA);
	int32 IntValueB = UnderlyingProperty->GetSignedIntPropertyValue(PropertyAddrB);

	if (IntValueA != IntValueB)
	{
		FString StringValueA = EnumDef->GetAuthoredNameStringByValue(IntValueA);
		FString StringValueB = EnumDef->GetAuthoredNameStringByValue(IntValueB);

		// for "Engine.Windows Target Settings.Default RHI", GetAuthoredNameStringByValue() returns "DefaultGraphicsRHI_DX12" which
		// is derived from the enum, but the UI displays "DirectX 12" from the 
		// metadata of the enum, declared like so:
		//UENUM()
		//enum class EDefaultGraphicsRHI : uint8
		//{
		//	DefaultGraphicsRHI_Default = 0 UMETA(DisplayName = "Default"),
		//	DefaultGraphicsRHI_DX11 = 1 UMETA(DisplayName = "DirectX 11"),
		//	DefaultGraphicsRHI_DX12 = 2 UMETA(DisplayName = "DirectX 12"),
		// 

		FText DisplayNameA = EnumDef->GetDisplayNameTextByIndex(IntValueA);
		FText DisplayNameB = EnumDef->GetDisplayNameTextByIndex(IntValueB);

		if (!DisplayNameA.IsEmpty() && DisplayNameA.ToString() != StringValueA)
		{
			StringValueA = DisplayNameA.ToString() + "(" + StringValueA + ")";
		}
		if (!DisplayNameB.IsEmpty() && DisplayNameB.ToString() != StringValueB)
		{
			StringValueB = DisplayNameB.ToString() + "(" + StringValueB + ")";
		}

		Report(PathA, PathB, "Enum", Property, StringValueA, StringValueB);
	}
}

void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FBoolProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	const bool ValueA = Property->GetPropertyValue(PropertyAddrA);
	const bool ValueB = Property->GetPropertyValue(PropertyAddrB);
	if (ValueA != ValueB)
	{
		FString StringValueA = ValueA ? TEXT("true") : TEXT("false");
		FString StringValueB = ValueB ? TEXT("true") : TEXT("false");

		Report(PathA, PathB, "Bool", Property, StringValueA, StringValueB);
	}
}

void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FNumericProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	// see if it's an enum
	UEnum* EnumDef = Property->GetIntPropertyEnum();
	if (EnumDef)
	{
		// export enums as strings
		int32 IntValueA = Property->GetSignedIntPropertyValue(PropertyAddrA);
		int32 IntValueB = Property->GetSignedIntPropertyValue(PropertyAddrB);

		if (IntValueA != IntValueB)
		{
			FString StringValueA = EnumDef->GetAuthoredNameStringByValue(IntValueA);
			FString StringValueB = EnumDef->GetAuthoredNameStringByValue(IntValueB);

			FText DisplayNameA = EnumDef->GetDisplayNameTextByIndex(IntValueA);
			FText DisplayNameB = EnumDef->GetDisplayNameTextByIndex(IntValueB);

			if (!DisplayNameA.IsEmpty() && DisplayNameA.ToString() != StringValueA)
			{
				StringValueA = DisplayNameA.ToString() + "(" + StringValueA + ")";
			}
			if (!DisplayNameB.IsEmpty() && DisplayNameB.ToString() != StringValueB)
			{
				StringValueB = DisplayNameB.ToString() + "(" + StringValueB + ")";
			}

			Report(PathA, PathB, "Numeric/Enum", Property, StringValueA, StringValueB);
		}
	}
	else if (Property->IsFloatingPoint())
	{
		const FString StringValueA = FString::SanitizeFloat(Property->GetFloatingPointPropertyValue(PropertyAddrA));
		const FString StringValueB = FString::SanitizeFloat(Property->GetFloatingPointPropertyValue(PropertyAddrB));
		if (StringValueA != StringValueB)
		{
			Report(PathA, PathB, "Numeric/float", Property, StringValueA, StringValueB);
		}
	}
	else if (Property->IsInteger())
	{
		const FString StringValueA = FString::FromInt(Property->GetSignedIntPropertyValue(PropertyAddrA));
		const FString StringValueB = FString::FromInt(Property->GetSignedIntPropertyValue(PropertyAddrB));
		if (StringValueA != StringValueB)
		{
			Report(PathA, PathB, "Numeric/int", Property, StringValueA, StringValueB);
		}
	}
	else
	{
		AddError( "No comparison done for Numeric/Unknown property " + Property->GetName() );
	}
}

void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FStrProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	const FString StringValueA = Property->GetPropertyValue(PropertyAddrA);
	const FString StringValueB = Property->GetPropertyValue(PropertyAddrB);
	if (StringValueA != StringValueB)
	{
		Report(PathA, PathB, "String", Property, Quote + StringValueA + Quote, Quote + StringValueB + Quote);
	}
}

void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FClassProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	auto A = Property->GetPropertyValue(PropertyAddrA);
	auto B = Property->GetPropertyValue(PropertyAddrB);
	const FString StringValueA = A ? A->GetName() : "NULL";
	const FString StringValueB = B ? B->GetName() : "NULL";
	if (StringValueA != StringValueB)
	{
		Report(PathA, PathB, "Class", Property, StringValueA, StringValueB);
	}
}


void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FTextProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	const FString StringValueA = Property->GetPropertyValue(PropertyAddrA).ToString();
	const FString StringValueB = Property->GetPropertyValue(PropertyAddrB).ToString();
	if (StringValueA != StringValueB)
	{
		Report(PathA, PathB, "Text", Property, Quote + StringValueA + Quote, Quote + StringValueB + Quote);
	}
}

void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FNameProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	const FString StringValueA = Property->GetPropertyValue(PropertyAddrA).ToString();
	const FString StringValueB = Property->GetPropertyValue(PropertyAddrB).ToString();
	if (StringValueA != StringValueB)
	{
		Report(PathA, PathB, "Name", Property, Quote + StringValueA + Quote, Quote + StringValueB + Quote);
	}
}

void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FObjectPtrProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	auto A = Property->GetPropertyValue(PropertyAddrA);
	auto B = Property->GetPropertyValue(PropertyAddrB);
	const FString StringValueA = A ? A->GetName() : "NULL";
	const FString StringValueB = B ? B->GetName() : "NULL";
	if (StringValueA != StringValueB)
	{
		Report(PathA, PathB, "Object", Property, StringValueA, StringValueB);
	}
}

void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FSoftObjectProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	FSoftObjectPtr A = Property->GetPropertyValue(PropertyAddrA);
	FSoftObjectPtr B = Property->GetPropertyValue(PropertyAddrB);
	const FString StringValueA = A.ToString();
	const FString StringValueB = B.ToString();
	if (StringValueA != StringValueB)
	{
		Report(PathA, PathB, "SoftObject", Property, StringValueA, StringValueB);
	}
}

void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FStructProperty* StructProperty, const uint8* StructAddrA, const uint8* StructAddrB)
{
	if (!StructProperty) return;

	UScriptStruct* Struct = StructProperty->Struct;

	if (Struct)
	{
		const FString PathAEx = PathA + "/" + Struct->GetName();
		const FString PathBEx = PathB + "/" + Struct->GetName();

		for (FProperty* Prop = Struct->PropertyLink; Prop != nullptr; Prop = Prop->PropertyLinkNext)
		{
			const uint8* PropertyAddrA = Prop->ContainerPtrToValuePtr<uint8>(StructAddrA, 0);
			const uint8* PropertyAddrB = Prop->ContainerPtrToValuePtr<uint8>(StructAddrB, 0);

			CompareProperty(PathAEx, PathBEx, Prop, PropertyAddrA, PropertyAddrB);
		}
	}
}


void UVehicleCompareImpl::Compare(const FString& PathA, const FString& PathB, FArrayProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	FScriptArrayHelper ArrayHelperA(Property, PropertyAddrA);
	FScriptArrayHelper ArrayHelperB(Property, PropertyAddrB);

	if (ArrayHelperA.Num() != ArrayHelperB.Num())
	{
		const FString PathAEx = AppendDisplayName(PathA, Property);
		const FString PathBEx = AppendDisplayName(PathB, Property);

		FString Message = PathAEx + " has " + FString::FromInt(ArrayHelperA.Num()) + " elements, " + PathBEx + " has " + FString::FromInt( ArrayHelperB.Num() );
		AddWarning(Message);
	}

	// this is the array type 
	if (Property->Inner)
	{
		// need to test all the things it might be an array of 
		if (FStructProperty* StructProperty = CastField<FStructProperty>(Property->Inner))
		{
			const int32 MinI = FGenericPlatformMath::Min(ArrayHelperA.Num(), ArrayHelperB.Num());

			for (int32 i = 0; i < MinI; ++i)
			{
				const uint8* StructAddressA = ArrayHelperA.GetRawPtr(i);
				const uint8* StructAddressB = ArrayHelperB.GetRawPtr(i);
				const FString Suffix = "/" + Property->GetName() + "[" + FString::FromInt(i) + "]";
				const FString PathAEx = PathA + Suffix;
				const FString PathBEx = PathB + Suffix;

				Compare(PathAEx, PathBEx, StructProperty, StructAddressA, StructAddressB);
			}
		}
		else if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property->Inner))
		{
			const int32 MinI = FGenericPlatformMath::Min(ArrayHelperA.Num(), ArrayHelperB.Num());

			for (int32 i = 0; i < MinI; ++i)
			{
				const uint8* DataAddressA = ArrayHelperA.GetRawPtr(i);
				const uint8* DataAddressB = ArrayHelperB.GetRawPtr(i);
				const FString Suffix = "/" + Property->GetName() + "[" + FString::FromInt(i) + "]";
				const FString PathAEx = PathA + Suffix;
				const FString PathBEx = PathB + Suffix;

				Compare(PathAEx, PathBEx, NumericProperty, DataAddressA, DataAddressB);
			}
		}
		else if (FNameProperty* NameProperty = CastField<FNameProperty>(Property->Inner))
		{
			const int32 MinI = FGenericPlatformMath::Min(ArrayHelperA.Num(), ArrayHelperB.Num());

			for (int32 i = 0; i < MinI; ++i)
			{
				const uint8* DataAddressA = ArrayHelperA.GetRawPtr(i);
				const uint8* DataAddressB = ArrayHelperB.GetRawPtr(i);
				const FString Suffix = "/" + Property->GetName() + "[" + FString::FromInt(i) + "]";
				const FString PathAEx = PathA + Suffix;
				const FString PathBEx = PathB + Suffix;

				Compare(PathAEx, PathBEx, NameProperty, DataAddressA, DataAddressB);
			}
		}
		else if (FObjectPtrProperty* ObjectProperty = CastField<FObjectPtrProperty>(Property->Inner))
		{
			const int32 MinI = FGenericPlatformMath::Min(ArrayHelperA.Num(), ArrayHelperB.Num());

			for (int32 i = 0; i < MinI; ++i)
			{
				const uint8* DataAddressA = ArrayHelperA.GetRawPtr(i);
				const uint8* DataAddressB = ArrayHelperB.GetRawPtr(i);
				const FString Suffix = "/" + Property->GetName() + "[" + FString::FromInt(i) + "]";
				const FString PathAEx = PathA + Suffix;
				const FString PathBEx = PathB + Suffix;

				Compare(PathAEx, PathBEx, ObjectProperty, DataAddressA, DataAddressB);
			}
		}
		else if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property->Inner))
		{
			const int32 MinI = FGenericPlatformMath::Min(ArrayHelperA.Num(), ArrayHelperB.Num());

			for (int32 i = 0; i < MinI; ++i)
			{
				const uint8* DataAddressA = ArrayHelperA.GetRawPtr(i);
				const uint8* DataAddressB = ArrayHelperB.GetRawPtr(i);
				const FString Suffix = "/" + Property->GetName() + "[" + FString::FromInt(i) + "]";
				const FString PathAEx = PathA + Suffix;
				const FString PathBEx = PathB + Suffix;

				Compare(PathAEx, PathBEx, EnumProperty, DataAddressA, DataAddressB);
			}
		}
		else
		{
			AddError( "No comparison done for " + Property->Inner->GetClass()->GetName() );
		}
	}
}




void UVehicleCompareImpl::CompareVehicleMovementComponents(
	const FString& PathA,
	const FString& PathB,
	const UChaosWheeledVehicleMovementComponent* A,
	const UChaosWheeledVehicleMovementComponent* B)
{
	if (!A) return;
	if (!B) return;

	AddInfo("Comparing vehicle movement componnets " + PathA + " with " + PathB);

	for (TFieldIterator<FProperty> It(UChaosWheeledVehicleMovementComponent::StaticClass()); It; ++It)
	{
		FProperty* Property = *It;

		if (Property->HasAnyPropertyFlags(EPropertyFlags::CPF_Edit))
		{
			const uint8* PropertyAddrA = Property->ContainerPtrToValuePtr<uint8>(A);
			const uint8* PropertyAddrB = Property->ContainerPtrToValuePtr<uint8>(B);

			CompareProperty(PathA, PathB, Property, PropertyAddrA, PropertyAddrB);
		}
	}
}


void UVehicleCompareImpl::CompareVehicleBlueprints(const FString& VehicleAssetPath1, const FString& VehicleAssetPath2)
{
	AddInfo("Comparing " + VehicleAssetPath1 + " with " + VehicleAssetPath2);

	TArray< FString > Paths = { VehicleAssetPath1, VehicleAssetPath2 };

	TArray< UBlueprint* > Blueprints;

	for (int i = 0; i < Paths.Num(); ++i)
	{
		UBlueprint* Blueprint = Cast<UBlueprint>(UEditorAssetLibrary::LoadAsset(Paths[i]));
		if (!Blueprint)
		{
			AddError( "Cannot load blueprint \"" + Paths[i] + "\"");
			return;
		}
		else
		{
			Blueprints.Add(Blueprint);
		}
	}

	USubobjectDataSubsystem* SubobjectDataSubsystem = USubobjectDataSubsystem::Get();

	TArray< TArray< FSubobjectDataHandle > > SubobjectDataHandles = { {}, {} };
	TArray< TArray< FSubobjectData > > SubobjectDatas = { {}, {} };
	TArray< TArray< const UObject* > > Subobjects = { {}, {} };
	TArray< TArray< const USkeletalMeshComponent* > > SkeletalMeshComponents = { {}, {} };
	TArray< TArray< const UChaosWheeledVehicleMovementComponent* > > VehicleMovementComponents = { {}, {} };

	for (int i = 0; i < Blueprints.Num(); ++i)
	{
		SubobjectDataSubsystem->K2_GatherSubobjectDataForBlueprint(Blueprints[i], SubobjectDataHandles[i]);
		for (const FSubobjectDataHandle& Handle : SubobjectDataHandles[i])
		{
			FSubobjectData Data;
			SubobjectDataSubsystem->K2_FindSubobjectDataFromHandle(Handle, Data);
			SubobjectDatas[i].Add(Data);

			const UObject* Object = USubobjectDataBlueprintFunctionLibrary::GetObject(Data);
			if (const UChaosWheeledVehicleMovementComponent* Comp = Cast< const UChaosWheeledVehicleMovementComponent >(Object))
			{
				VehicleMovementComponents[i].Add(Comp);
			}

			if (const USkeletalMeshComponent* Skel = Cast< const USkeletalMeshComponent >(Object))
			{
				SkeletalMeshComponents[i].Add(Skel);
			}

			Subobjects[i].Add(Object);
		}
	}

	bool PrintComponentList = false;

	if (Subobjects[0].Num() != Subobjects[1].Num())
	{
		AddWarning("Blueprint subobject (component) count is different");
		PrintComponentList = true;
	}

	if (SkeletalMeshComponents[0].Num() != SkeletalMeshComponents[1].Num())
	{
		AddWarning("Blueprint skeletal mesh components count is different");
		PrintComponentList = true;
	}

	if (VehicleMovementComponents[0].Num() != VehicleMovementComponents[1].Num())
	{
		AddWarning("Blueprint vehicle movement components count is different");
		PrintComponentList = true;
	}

	if (PrintComponentList)
	{
		for (int i = 0; i < Blueprints.Num(); ++i)
		{
			AddWarning("Blueprint subobjects for " + Blueprints[i]->GetFName().ToString());

			for (const UObject* Object : Subobjects[i])
			{
				AddWarning("   subobject " + Object->GetFName().ToString());
			}
		}
	}

	FString Temp;
	FString PathA;
	FString PathB;

	if (!VehicleAssetPath1.Split("/", &Temp, &PathA, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
	{
		PathA = VehicleAssetPath1;
	}

	if (!VehicleAssetPath2.Split("/", &Temp, &PathB, ESearchCase::CaseSensitive, ESearchDir::FromEnd))
	{
		PathB = VehicleAssetPath2;
	}

	// compare vehicle movement components
	int32 MinI = FGenericPlatformMath::Min(VehicleMovementComponents[0].Num(), VehicleMovementComponents[1].Num());

	for (int i = 0; i < MinI; ++i)
	{
		const FString NameA = VehicleMovementComponents[0][i]->GetName();
		const FString NameB = VehicleMovementComponents[1][i]->GetName();
		CompareVehicleMovementComponents(PathA + "/" + NameA, PathB + "/" + NameB, VehicleMovementComponents[0][i], VehicleMovementComponents[1][i]);
	}

	// compare skeletal mesh components
	MinI = FGenericPlatformMath::Min(SkeletalMeshComponents[0].Num(), SkeletalMeshComponents[1].Num());

	for (int i = 0; i < MinI; ++i)
	{
		const FString NameA = SkeletalMeshComponents[0][i]->GetName();
		const FString NameB = SkeletalMeshComponents[1][i]->GetName();
		CompareSkeletalMeshComponents(PathA + "/" + NameA, PathB + "/" + NameB, SkeletalMeshComponents[0][i], SkeletalMeshComponents[1][i]);
	}

	// compare wheel bone names with bones names in skeleton
	for (int Vehicle = 0; Vehicle < 2; ++Vehicle)
	{
		MinI = FGenericPlatformMath::Min(SkeletalMeshComponents[Vehicle].Num(), VehicleMovementComponents[Vehicle].Num());

		for (int i = 0; i < MinI; ++i)
		{
			CheckWheelNames(Paths[Vehicle], SkeletalMeshComponents[Vehicle][i], VehicleMovementComponents[Vehicle][i]);
		}
	}

}


void UVehicleCompareImpl::CompareSkeletalMeshComponents(const FString& PathA, const FString& PathB, const class USkeletalMeshComponent* A, const USkeletalMeshComponent* B)
{
	if (!A) return;
	if (!B) return;

	AddInfo("Comparing skeletal mesh components " + PathA + " with " + PathB);

	for (TFieldIterator<FProperty> It(USkeletalMeshComponent::StaticClass()); It; ++It)
	{
		FProperty* Property = *It;

		if (Property->HasAnyPropertyFlags(EPropertyFlags::CPF_Edit))
		{
			const uint8* PropertyAddrA = Property->ContainerPtrToValuePtr<uint8>(A);
			const uint8* PropertyAddrB = Property->ContainerPtrToValuePtr<uint8>(B);

			CompareProperty(PathA, PathB, Property, PropertyAddrA, PropertyAddrB);
		}
	}
}


const TArray<TSharedRef<FDifference>> & UVehicleCompareImpl::GetResults() const
{
	return Results;
}

void UVehicleCompareImpl::AddMessage(const FString& Message, const EDifferenceType& Type)
{
	TSharedRef<FDifference> Diff = MakeShared<FDifference>();
	Diff->Type = Type;
	Diff->Message = Message;
	Results.Add(Diff);
}

void UVehicleCompareImpl::AddWarning(const FString& Message)
{
	AddMessage(Message, EDifferenceType::Warning);
}

void UVehicleCompareImpl::AddError(const FString& Message)
{
	AddMessage(Message, EDifferenceType::Error);
}

void UVehicleCompareImpl::AddInfo(const FString& Message)
{
	AddMessage(Message, EDifferenceType::Info );
}

void UVehicleCompareImpl::CompareProperty(const FString& PathA, const FString& PathB, FProperty* Property, const uint8* PropertyAddrA, const uint8* PropertyAddrB)
{
	// cast to every possible class, thats the way its done in the engine
	if (FEnumProperty* TypedProperty = CastField<FEnumProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FBoolProperty* TypedProperty = CastField<FBoolProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FNumericProperty* TypedProperty = CastField<FNumericProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FStrProperty* TypedProperty = CastField<FStrProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FTextProperty* TypedProperty = CastField<FTextProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FArrayProperty* TypedProperty = CastField<FArrayProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FStructProperty* TypedProperty = CastField<FStructProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FClassProperty* TypedProperty = CastField<FClassProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FNameProperty* TypedProperty = CastField<FNameProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FObjectPtrProperty* TypedProperty = CastField<FObjectPtrProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else if (FSoftObjectProperty* TypedProperty = CastField<FSoftObjectProperty>(Property))
	{
		Compare(PathA, PathB, TypedProperty, PropertyAddrA, PropertyAddrB);
	}
	else {
		AddError( "No comparison done for property " + Property->GetName()) ;
	}
}

// check the BP_Car ... BoneNames has wheel names for those names used in the ChaosWheeledVehicleMovementComponent->WheelSetup
void UVehicleCompareImpl::CheckWheelNames(const FString& Path, const USkeletalMeshComponent* SkeletalMeshComponent, const UChaosWheeledVehicleMovementComponent* VehicleMovementComponent)
{

	if (!SkeletalMeshComponent) return;
	if (!VehicleMovementComponent) return;

	UPhysicsAsset* PhysicsAsset = SkeletalMeshComponent->GetPhysicsAsset();
	if (!PhysicsAsset)
	{
		AddError("No physics asset for skeletal mesh component " + Path );
		return;
	}


	// BP_Car->SkeletalMeshComponent->SkeletalMesh->Skeleton->FReferenceSkeleton->BoneInfo

	USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->GetSkeletalMeshAsset();
	if(!SkeletalMesh)
	{
		AddError("No skeletal mesh for skeletal mesh component " + Path );
		return;
	}

	USkeleton* Skeleton = SkeletalMesh->GetSkeleton();
	if (!Skeleton)
	{
		AddError("No skeleton for skeletal mesh component " + Path);
		return;
	}

	const FReferenceSkeleton Ref = Skeleton->GetReferenceSkeleton();

	const TArray<FMeshBoneInfo>& BoneInfo = Ref.GetRefBoneInfo(); // or GetRawRefBoneInfo() ??

	TArray<FName> BoneNames;

	for ( const FMeshBoneInfo& Info: BoneInfo)
	{
		BoneNames.Add(Info.Name);
	}

	// now check the wheels 

	const TArray<FChaosWheelSetup>& WheelSetups = VehicleMovementComponent->WheelSetups;
	int count = 0;
	for (const FChaosWheelSetup& WheelSetup : WheelSetups)
	{
		if (WheelSetup.BoneName.IsNone())
		{
			AddWarning("Wheel setup [" + FString::FromInt(count) + " has NONE for bone name");
		}
		else if (WheelSetup.BoneName == "")
		{
			AddWarning("Wheel setup [" + FString::FromInt(count) + " has empty bone name");
		}
		else
		{
			const bool bExists = BoneNames.Contains(WheelSetup.BoneName);
			if (!bExists)
			{
				AddWarning( Path + " Wheel setup [" + FString::FromInt(count) + "] has bone name \"" + *WheelSetup.BoneName.ToString() + "\", this bone does not exist on the skeletal mesh " + SkeletalMesh->GetPathName());
			}

		}
	}
}
