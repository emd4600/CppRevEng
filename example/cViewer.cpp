#include "stdafx.h"
#include "cViewer.h"

// This class defines all methods for cViewer, so we can use them anywhere in our code normally
// Since we declared cViewer inside namespace App, use it here too
// Visual Studio Intellisense or whatever will show warnings, but you can ignore them

namespace App
{
	// bool Initialize(bool);
	RedirectMethod(cViewer, Initialize, bool, Args(bool arg0), Args(arg0));

	// bool ClearScene(int8_t flags);
	RedirectMethod(cViewer, ClearScene, bool, Args(int8_t flags), Args(flags));

	// bool LoadTransformations();
	RedirectMethod_noargs(cViewer, LoadTransformations, bool);

	// void SetBackgroundColor(const ColorRGBA& color);
	RedirectMethod(cViewer, SetBackgroundColor, void, Args(const ColorRGBA& color), Args(color));

	// float GetNearPlane() const;
	// float GetFarPlane() const;
	RedirectMethod_noargs_const(cViewer, GetNearPlane, float);
	RedirectMethod_noargs_const(cViewer, GetFarPlane, float);

	// void SetNearPlane(float fValue);
	// void SetFarPlane(float fValue);
	RedirectMethod(cViewer, SetNearPlane, void, Args(float value), Args(value));
	RedirectMethod(cViewer, SetFarPlane, void, Args(float value), Args(value));

	// void SetAspectRatio(float fValue);
	RedirectMethod(cViewer, SetAspectRatio, void, Args(float value), Args(value));

	// void SetCameraMaterialLODs(const Vector4& values);
	RedirectMethod(cViewer, SetCameraMaterialLODs, void, Args(const Vector4& values), Args(values));

	// Vector4 GetCameraMaterialLODs() const;
	// Be aware with this one: sizeof(Vector4) > 8, so use structret
	RedirectMethod_noargs_structret_const(cViewer, GetCameraMaterialLODs, Vector4);

	// void SetRenderType(int renderType, bool = 0);
	RedirectMethod(cViewer, SetRenderType, void, Args(int renderType, bool arg1), Args(renderType, arg1));

	// bool GetCameraToMouse(Vector3& cameraPosition, Vector3& direction) const;
	RedirectMethod_const(cViewer, GetCameraToMouse, bool,
		Args(Vector3& cameraPosition, Vector3& direction), Args(cameraPosition, direction));

	// bool GetCameraToPoint(float x, float y, Vector3& cameraPosition, Vector3& direction) const;
	RedirectMethod_const(cViewer, GetCameraToPoint, bool,
		Args(float x, float y, Vector3& cameraPosition, Vector3& direction), Args(x, y, cameraPosition, direction));
}