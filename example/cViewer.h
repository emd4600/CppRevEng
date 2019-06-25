#pragma once

#include <CppRevEng.h>

namespace App
{
	struct Vector3 {
		float x;
		float y;
		float z;
		float w;
	};

	struct Vector4 {
		float x;
		float y;
		float z;
		float w;
	};

	struct ColorRGBA {
		float r;
		float g;
		float b;
		float a;
	};

	struct Matrix4 {
		float m[4][4];
	};

	class cViewer
	{
	public:
		// Methods we have found by reverse engineering

		bool Initialize(bool);

		bool ClearScene(int8_t flags);

		bool LoadTransformations();

		void SetBackgroundColor(const ColorRGBA& color);

		float GetNearPlane() const;
		float GetFarPlane() const;

		void SetNearPlane(float fValue);
		void SetFarPlane(float fValue);

		void SetAspectRatio(float fValue);

		void SetCameraMaterialLODs(const Vector4& values);
		Vector4 GetCameraMaterialLODs() const;

		void SetRenderType(int renderType, bool = 0);

		bool GetCameraToMouse(Vector3& cameraPosition, Vector3& direction) const;

		bool GetCameraToPoint(float x, float y, Vector3& cameraPosition, Vector3& direction) const;

	// It's recommended that, when reverse engineering, you only use public & protected:
	// this way, you still can access all fields when detouring
	protected:
		/* 00h */	Matrix4 mViewTransform;
		/* 40h */	Matrix4 field_40;
		/* 80h */	Matrix4 field_80;
		/* C0h */	Matrix4 field_C0;
		/* 100h */	Matrix4 field_100;
		/* 140h */	ColorRGBA mBackgroundColor;
		/* 150h */	int mRenderType;  // 0xF -> hologram!
		/* 154h */	bool field_154;  // related to renderType, a char/bool?
		/* 158h */	int field_158;
		/* 15Ch */	float mCameraMaterialLODs[4];
		/* 16Ch */	bool field_16C;
		/* 170h */	void* mCamera;
	};

	namespace Addresses(cViewer) {
		// The addresses are from the latest disk version of Spore
		DeclareAddress(Initialize, 0x7C5470);
		DeclareAddress(ClearScene, 0x7C42F0);
		DeclareAddress(LoadTransformations, 0x7C5650);
		DeclareAddress(SetBackgroundColor, 0x7C42C0);
		DeclareAddress(GetNearPlane, 0x7C4330);
		DeclareAddress(GetFarPlane, 0x7C4340);
		DeclareAddress(SetNearPlane, 0x7C5240);
		DeclareAddress(SetFarPlane, 0x7C5260);
		DeclareAddress(SetAspectRatio, 0x7C51F0);
		DeclareAddress(SetCameraMaterialLODs, 0x7C43A0);
		DeclareAddress(GetCameraMaterialLODs, 0x7C4B80);
		DeclareAddress(SetRenderType, 0x7C4380);
		DeclareAddress(GetCameraToMouse, 0x7C4FA0);
		DeclareAddress(GetCameraToPoint, 0x7C4FA0);
	}
}