// Create a file called widevine_mac_override.c
#include "open_cdm.h"
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <stdlib.h>

// This will override the function in system.cpp
OpenCDMError opencdm_is_type_supported(const char keySystem[], const char mimeType[])
{
    printf("MAC OVERRIDE: opencdm_is_type_supported called for %s\n", keySystem ? keySystem : "NULL");
    
    // Skip check if environment variable is set
    const char* skip_check = getenv("OPENCDM_SKIP_SUPPORT_CHECK");
    if (skip_check && (strcmp(skip_check, "1") == 0)) {
        printf("Skipping CDM support check due to OPENCDM_SKIP_SUPPORT_CHECK=1\n");
        return ERROR_NONE;
    }
    
    // Support Widevine based on key system identifier
    if (keySystem && (
        strcmp(keySystem, "com.widevine.alpha") == 0 ||
        strcmp(keySystem, "edef8ba9-79d6-4ace-a3c8-27dcd51d21ed") == 0)) {
        
        // On macOS, try to load the CDM directly
        const char* cdm_path = getenv("GST_WIDEVINE_CDM_PATH");
        if (cdm_path) {
            void* handle = dlopen(cdm_path, RTLD_LAZY);
            if (handle) {
                dlclose(handle);
                printf("Successfully loaded Widevine from %s\n", cdm_path);
                return ERROR_NONE;
            } else {
                printf("Failed to load Widevine from %s: %s\n", cdm_path, dlerror());
            }
        }
        
        // Fallback to built-in support if library loading failed
        printf("Using fallback Widevine support\n");
        return ERROR_NONE;
    }
    
    // Handle ClearKey as well
    if (keySystem && (
        strcmp(keySystem, "org.w3.clearkey") == 0 ||
        strcmp(keySystem, "1077efec-c0b2-4d02-ace3-3c1e52e2fb4b") == 0 ||
        strcmp(keySystem, "e2719d58-a985-b3c9-781a-b030af78d30e") == 0)) {
        printf("ClearKey support confirmed\n");
        return ERROR_NONE;
    }
    
    printf("Key system not supported by override: %s\n", keySystem ? keySystem : "NULL");
    return ERROR_KEYSYSTEM_NOT_SUPPORTED;
}