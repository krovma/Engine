#pragma once
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define UNUSED(x) (void)(x);
#define STATIC //Nothing
#define FUNCTION __FUNCDNAME__
#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }


class NamedStrings;
extern NamedStrings g_gameConfigs;
