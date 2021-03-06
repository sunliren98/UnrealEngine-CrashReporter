// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "OculusFunctionLibrary.h"
#include "OculusHMDPrivate.h"
#include "OculusHMD.h"

//-------------------------------------------------------------------------------------------------
// UOculusFunctionLibrary
//-------------------------------------------------------------------------------------------------

UOculusFunctionLibrary::UOculusFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

OculusHMD::FOculusHMD* UOculusFunctionLibrary::GetOculusHMD()
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	if (GEngine && GEngine->HMDDevice.IsValid())
	{
		auto HMDType = GEngine->HMDDevice->GetHMDDeviceType();
		if (HMDType == EHMDDeviceType::DT_OculusRift || HMDType == EHMDDeviceType::DT_GearVR)
		{
			return static_cast<OculusHMD::FOculusHMD*>(GEngine->HMDDevice.Get());
		}
	}
#endif
	return nullptr;
}

void UOculusFunctionLibrary::GetPose(FRotator& DeviceRotation, FVector& DevicePosition, FVector& NeckPosition, bool bUseOrienationForPlayerCamera, bool bUsePositionForPlayerCamera, const FVector PositionScale)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD && OculusHMD->IsHeadTrackingAllowed())
	{
		FQuat OrientationAsQuat;

		OculusHMD->GetCurrentHMDPose(OrientationAsQuat, DevicePosition, bUseOrienationForPlayerCamera, bUsePositionForPlayerCamera, PositionScale);

		DeviceRotation = OrientationAsQuat.Rotator();

		NeckPosition = OculusHMD->GetNeckPosition(OrientationAsQuat, DevicePosition, PositionScale);

		//UE_LOG(LogUHeadMountedDisplay, Log, TEXT("POS: %.3f %.3f %.3f"), DevicePosition.X, DevicePosition.Y, DevicePosition.Z);
		//UE_LOG(LogUHeadMountedDisplay, Log, TEXT("NECK: %.3f %.3f %.3f"), NeckPosition.X, NeckPosition.Y, NeckPosition.Z);
		//UE_LOG(LogUHeadMountedDisplay, Log, TEXT("ROT: sYaw %.3f Pitch %.3f Roll %.3f"), DeviceRotation.Yaw, DeviceRotation.Pitch, DeviceRotation.Roll);
	}
	else
#endif // #if OCULUS_HMD_SUPPORTED_PLATFORMS
	{
		DeviceRotation = FRotator::ZeroRotator;
		DevicePosition = FVector::ZeroVector;
	}
}

void UOculusFunctionLibrary::SetBaseRotationAndBaseOffsetInMeters(FRotator Rotation, FVector BaseOffsetInMeters, EOrientPositionSelector::Type Options)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		if ((Options == EOrientPositionSelector::Orientation) || (Options == EOrientPositionSelector::OrientationAndPosition))
		{
			OculusHMD->SetBaseRotation(Rotation);
		}
		if ((Options == EOrientPositionSelector::Position) || (Options == EOrientPositionSelector::OrientationAndPosition))
		{
			OculusHMD->SetBaseOffsetInMeters(BaseOffsetInMeters);
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::GetBaseRotationAndBaseOffsetInMeters(FRotator& OutRotation, FVector& OutBaseOffsetInMeters)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OutRotation = OculusHMD->GetBaseRotation();
		OutBaseOffsetInMeters = OculusHMD->GetBaseOffsetInMeters();
	}
	else
	{
		OutRotation = FRotator::ZeroRotator;
		OutBaseOffsetInMeters = FVector::ZeroVector;
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::GetRawSensorData(FVector& AngularAcceleration, FVector& LinearAcceleration, FVector& AngularVelocity, FVector& LinearVelocity, float& TimeInSeconds, ETrackedDeviceType DeviceType)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr && OculusHMD->IsHMDActive())
	{
		ovrpPoseStatef state;		
		if (OVRP_SUCCESS(ovrp_GetNodePoseState2(ovrpStep_Game, OculusHMD::ToOvrpNode(DeviceType), &state)))
		{
			AngularAcceleration = OculusHMD::ToFVector(state.AngularAcceleration);
			LinearAcceleration = OculusHMD::ToFVector(state.Acceleration);
			AngularVelocity = OculusHMD::ToFVector(state.AngularVelocity);
			LinearVelocity = OculusHMD::ToFVector(state.Velocity);
			TimeInSeconds = state.Time;
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

bool UOculusFunctionLibrary::IsDeviceTracked(ETrackedDeviceType DeviceType)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr && OculusHMD->IsHMDActive())
	{
		ovrpBool present;
		if (OVRP_SUCCESS(ovrp_GetNodePresent2(OculusHMD::ToOvrpNode(DeviceType), &present)))
		{
			return present == ovrpBool_True;
		} 
		else
		{
			return false;
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
	return false;
}

void UOculusFunctionLibrary::SetCPUAndGPULevels(int CPULevel, int GPULevel)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr && OculusHMD->IsHMDActive())
	{
		ovrp_SetSystemCpuLevel2(CPULevel);
		ovrp_SetSystemGpuLevel2(GPULevel);
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

bool UOculusFunctionLibrary::GetUserProfile(FHmdUserProfile& Profile)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FOculusHMD::UserProfile Data;
		if (OculusHMD->GetUserProfile(Data))
		{
			Profile.Name = "";
			Profile.Gender = "Unknown";
			Profile.PlayerHeight = 0.0f;
			Profile.EyeHeight = Data.EyeHeight;
			Profile.IPD = Data.IPD;
			Profile.NeckToEyeDistance = FVector2D(Data.EyeDepth, 0.0f);
			return true;
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
	return false;
}

void UOculusFunctionLibrary::SetBaseRotationAndPositionOffset(FRotator BaseRot, FVector PosOffset, EOrientPositionSelector::Type Options)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		if (Options == EOrientPositionSelector::Orientation || Options == EOrientPositionSelector::OrientationAndPosition)
		{
			GEngine->HMDDevice->SetBaseRotation(BaseRot);
		}
		if (Options == EOrientPositionSelector::Position || Options == EOrientPositionSelector::OrientationAndPosition)
		{
			OculusHMD->GetSettings()->PositionOffset = PosOffset;
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::GetBaseRotationAndPositionOffset(FRotator& OutRot, FVector& OutPosOffset)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OutRot = OculusHMD->GetBaseRotation();
		OutPosOffset = OculusHMD->GetSettings()->PositionOffset;
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::AddLoadingSplashScreen(class UTexture2D* Texture, FVector TranslationInMeters, FRotator Rotation, FVector2D SizeInMeters, FRotator DeltaRotation, bool bClearBeforeAdd)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			if (bClearBeforeAdd)
			{
				Splash->ClearSplashes();
			}
			Splash->SetLoadingIconMode(false);

			FOculusSplashDesc Desc;
			Desc.LoadingTexture = Texture;
			Desc.QuadSizeInMeters = SizeInMeters;
			Desc.TransformInMeters = FTransform(Rotation, TranslationInMeters);
			Desc.DeltaRotation = FQuat(DeltaRotation);
			Splash->AddSplash(Desc);
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::ClearLoadingSplashScreens()
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			Splash->ClearSplashes();
			Splash->SetLoadingIconMode(false);
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::ShowLoadingSplashScreen()
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			Splash->SetLoadingIconMode(false);
			Splash->Show();
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::HideLoadingSplashScreen(bool bClear)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			Splash->Hide();
			if (bClear)
			{
				Splash->ClearSplashes();
			}
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::EnableAutoLoadingSplashScreen(bool bAutoShowEnabled)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			Splash->SetAutoShow(bAutoShowEnabled);
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

bool UOculusFunctionLibrary::IsAutoLoadingSplashScreenEnabled()
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			return Splash->IsAutoShow();
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
	return false;
}

void UOculusFunctionLibrary::ShowLoadingIcon(class UTexture2D* Texture)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			Splash->ClearSplashes();
			FOculusSplashDesc Desc;
			Desc.LoadingTexture = Texture;
			Splash->AddSplash(Desc);
			Splash->SetLoadingIconMode(true);
			Splash->Show();
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::HideLoadingIcon()
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			Splash->Hide();
			Splash->ClearSplashes();
			Splash->SetLoadingIconMode(false);
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

bool UOculusFunctionLibrary::IsLoadingIconEnabled()
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			return Splash->IsLoadingIconMode();
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
	return false;
}


void UOculusFunctionLibrary::SetLoadingSplashParams(FString TexturePath, FVector DistanceInMeters, FVector2D SizeInMeters, FVector RotationAxis, float RotationDeltaInDeg)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			Splash->ClearSplashes();
			Splash->SetLoadingIconMode(false);
			FOculusSplashDesc Desc;
			Desc.TexturePath = TexturePath;
			Desc.QuadSizeInMeters = SizeInMeters;
			Desc.TransformInMeters = FTransform(DistanceInMeters);
			Desc.DeltaRotation = FQuat(RotationAxis, FMath::DegreesToRadians(RotationDeltaInDeg));
			Splash->AddSplash(Desc);
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

void UOculusFunctionLibrary::GetLoadingSplashParams(FString& TexturePath, FVector& DistanceInMeters, FVector2D& SizeInMeters, FVector& RotationAxis, float& RotationDeltaInDeg)
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		OculusHMD::FSplash* Splash = OculusHMD->GetSplash();
		if (Splash)
		{
			FOculusSplashDesc Desc;
			if (Splash->GetSplash(0, Desc))
			{
				if (Desc.LoadingTexture && Desc.LoadingTexture->IsValidLowLevel())
				{
					TexturePath = Desc.LoadingTexture->GetPathName();
				}
				else
				{
					TexturePath = Desc.TexturePath.ToString();
				}
				DistanceInMeters = Desc.TransformInMeters.GetTranslation();
				SizeInMeters	 = Desc.QuadSizeInMeters;

				const FQuat rotation(Desc.DeltaRotation);
				rotation.ToAxisAndAngle(RotationAxis, RotationDeltaInDeg);
			}
		}
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
}

class IStereoLayers* UOculusFunctionLibrary::GetStereoLayers()
{
#if OCULUS_HMD_SUPPORTED_PLATFORMS
	OculusHMD::FOculusHMD* OculusHMD = GetOculusHMD();
	if (OculusHMD != nullptr)
	{
		return OculusHMD;
	}
#endif // OCULUS_HMD_SUPPORTED_PLATFORMS
	return nullptr;
}

DECLARE_FUNCTION(UOculusFunctionLibrary::execIsPowerLevelStateMinimum)
{
	P_FINISH;

	FBlueprintExceptionInfo ExceptionInfo(
		EBlueprintExceptionType::AccessViolation,
		FText::Format(NSLOCTEXT("OculusFuncLib", "DeprecatedGearVRFunc", "The Oculus API no longer supports this GearVR function ({0}). Please remove it from your Blueprint."), FText::FromString(TEXT("IsPowerLevelStateMinimum")))
	);
	FBlueprintCoreDelegates::ThrowScriptException(this, Stack, ExceptionInfo);

	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result = false;
	P_NATIVE_END;
}

DECLARE_FUNCTION(UOculusFunctionLibrary::execIsPowerLevelStateThrottled)
{
	P_FINISH;

	FBlueprintExceptionInfo ExceptionInfo(
		EBlueprintExceptionType::AccessViolation,
		FText::Format(NSLOCTEXT("OculusFuncLib", "DeprecatedGearVRFunc", "The Oculus API no longer supports this GearVR function ({0}). Please remove it from your Blueprint."), FText::FromString(TEXT("IsPowerLevelStateThrottled")))
	);
	FBlueprintCoreDelegates::ThrowScriptException(this, Stack, ExceptionInfo);

	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result = false;
	P_NATIVE_END;
}

DECLARE_FUNCTION(UOculusFunctionLibrary::execAreHeadPhonesPluggedIn)
{
	P_FINISH;

	FBlueprintExceptionInfo ExceptionInfo(
		EBlueprintExceptionType::AccessViolation,
		FText::Format(NSLOCTEXT("OculusFuncLib", "DeprecatedGearVRFunc", "The Oculus API no longer supports this GearVR function ({0}). Please remove it from your Blueprint."), FText::FromString(TEXT("AreHeadPhonesPluggedIn")))
	);
	FBlueprintCoreDelegates::ThrowScriptException(this, Stack, ExceptionInfo);

	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result = false;
	P_NATIVE_END;
}

DECLARE_FUNCTION(UOculusFunctionLibrary::execGetTemperatureInCelsius)
{
	P_FINISH;

	FBlueprintExceptionInfo ExceptionInfo(
		EBlueprintExceptionType::AccessViolation,
		FText::Format(NSLOCTEXT("OculusFuncLib", "DeprecatedGearVRFunc", "The Oculus API no longer supports this GearVR function ({0}). Please remove it from your Blueprint."), FText::FromString(TEXT("GetTemperatureInCelsius")))
	);
	FBlueprintCoreDelegates::ThrowScriptException(this, Stack, ExceptionInfo);

	P_NATIVE_BEGIN;
	*(float*)Z_Param__Result = 0.f;
	P_NATIVE_END;
}

DECLARE_FUNCTION(UOculusFunctionLibrary::execGetBatteryLevel)
{
	P_FINISH;

	FBlueprintExceptionInfo ExceptionInfo(
		EBlueprintExceptionType::AccessViolation,
		FText::Format(NSLOCTEXT("OculusFuncLib", "DeprecatedGearVRFunc", "The Oculus API no longer supports this GearVR function ({0}). Please remove it from your Blueprint."), FText::FromString(TEXT("GetBatteryLevel")))
	);
	FBlueprintCoreDelegates::ThrowScriptException(this, Stack, ExceptionInfo);

	P_NATIVE_BEGIN;
	*(float*)Z_Param__Result = 0.f;
	P_NATIVE_END;
}

DECLARE_FUNCTION(UOculusFunctionLibrary::execGetGearVRControllerHandedness)
{
	P_FINISH;

	FBlueprintExceptionInfo ExceptionInfo(
		EBlueprintExceptionType::AccessViolation,
		FText::Format(NSLOCTEXT("OculusFuncLib", "DeprecatedGearVRFunc", "The Oculus API no longer supports this GearVR function ({0}). Please remove it from your Blueprint."), FText::FromString(TEXT("GetGearVRControllerHandedness")))
	);
	FBlueprintCoreDelegates::ThrowScriptException(this, Stack, ExceptionInfo);

	P_NATIVE_BEGIN;
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	*(EGearVRControllerHandedness_DEPRECATED*)Z_Param__Result = EGearVRControllerHandedness_DEPRECATED::Unknown_DEPRECATED;
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
	P_NATIVE_END;
}

DECLARE_FUNCTION(UOculusFunctionLibrary::execEnableArmModel)
{
	P_GET_UBOOL(Z_Param_bArmModelEnable);
	P_FINISH;

	FBlueprintExceptionInfo ExceptionInfo(
		EBlueprintExceptionType::AccessViolation,
		FText::Format(NSLOCTEXT("OculusFuncLib", "DeprecatedGearVRFunc", "The Oculus API no longer supports this GearVR function ({0}). Please remove it from your Blueprint."), FText::FromString(TEXT("EnableArmModel")))
	);
	FBlueprintCoreDelegates::ThrowScriptException(this, Stack, ExceptionInfo);
}

DECLARE_FUNCTION(UOculusFunctionLibrary::execIsControllerActive)
{
	P_FINISH;

	FBlueprintExceptionInfo ExceptionInfo(
		EBlueprintExceptionType::AccessViolation,
		FText::Format(NSLOCTEXT("OculusFuncLib", "DeprecatedGearVRFunc", "The Oculus API no longer supports this GearVR function ({0}). Please remove it from your Blueprint."), FText::FromString(TEXT("IsControllerActive")))
	);
	FBlueprintCoreDelegates::ThrowScriptException(this, Stack, ExceptionInfo);

	P_NATIVE_BEGIN;
	*(bool*)Z_Param__Result = false;
	P_NATIVE_END;
}