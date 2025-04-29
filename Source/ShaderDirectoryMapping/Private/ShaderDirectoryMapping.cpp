// Copyright tdxh. All Rights Reserved.

#pragma once

#include "ShaderDirectoryMapping.h"

#include "ShaderDirectoryMappingSettings.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"
#include "Misc/MessageDialog.h"
#include "HAL/PlatformFilemanager.h"

#define LOCTEXT_NAMESPACE "FShaderDirectoryMappingModule"

DEFINE_LOG_CATEGORY(LogShaderDirectoryMapping);

void FShaderDirectoryMappingModule::StartupModule()
{
	UE_LOG(LogShaderDirectoryMapping, Log, TEXT("ShaderDirectoryMapping Plugin starting up"));
	ApplyShaderMappings();
}

void FShaderDirectoryMappingModule::ShutdownModule()
{
	UE_LOG(LogShaderDirectoryMapping, Log, TEXT("ShaderDirectoryMapping Plugin shutting down, you need to restart Unreal Editor"));
}

void FShaderDirectoryMappingModule::ApplyShaderMappings()
{
	const UShaderDirectoryMappingSettings* Settings = GetDefault<UShaderDirectoryMappingSettings>();
    
	// Validate Paths
	FString ErrorMessage;
	if (!Settings->ValidatePaths(ErrorMessage))
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(ErrorMessage));
		UE_LOG(LogShaderDirectoryMapping, Error, TEXT("%s"), *ErrorMessage);
		return;
	}

	// apply the Shaders path
	TArray<FShaderDirectoryMapping> MappingToBeApplied;
	
	if (Settings->bUseDefaultProjectShaders)
	{
		FShaderDirectoryMapping DefaultProjectShaders;

		DefaultProjectShaders.bEnabled = true;
		DefaultProjectShaders.VirtualPath = Settings->DefaultVirtualPath;
		DefaultProjectShaders.TargetPath = Settings->DefaultTargetPath;
		DefaultProjectShaders.TargetPathType = EShaderPathType::ProjectRelative;

		MappingToBeApplied.Emplace(DefaultProjectShaders);
	}

	MappingToBeApplied.Append(Settings->CustomShaderMappings);
	
	for (const FShaderDirectoryMapping& Mapping : MappingToBeApplied)
	{
		if (Mapping.bEnabled)
		{
			FString RealPath = Mapping.GetRealPath();
			AddShaderSourceDirectoryMapping(Mapping.VirtualPath, RealPath);
			UE_LOG(LogShaderDirectoryMapping, Log, TEXT("Mapped shader directory: %s -> %s"), 
				*Mapping.VirtualPath, *RealPath);
		}
	}
}
 

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FShaderDirectoryMappingModule, ShaderDirectoryMapping)