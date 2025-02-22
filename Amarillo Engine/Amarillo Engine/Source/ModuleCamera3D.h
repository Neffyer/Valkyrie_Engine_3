#pragma once
#include "Module.h"
#include "Globals.h"
#include "../External/MathGeoLib/include/Geometry/Frustum.h"
#include "..\External\MathGeoLib/include/Math/float3.h"
#include "..\External\MathGeoLib/include/Math/float4x4.h"
#include <vector>
#include "RenderTexture.h"

#include "..\External\ImGui/imgui_internal.h"

class Camera3D
{
public:
	Camera3D();

	void SetPosition(const float3& pos);
	const float3 GetPosition();

	float3 GetXDir();
	float3 GetZDir();
	float3 GetYDir();


	void GetCorners(float3* corners);
	void SetNearPlaneDistance(const float& set);
	void SetFarPlaneDistance(const float& set);
	void SetFOV(const float& set);
	void SetAspectRatio();

	const float GetNearPlaneDistance() const;
	const float GetFarPlaneDistance() const;
	const float GetVerticalFOV() const;
	const float GetHorizontalFOV() const;
	float* GetViewMatrix();
	float* GetProjectionMatrix();

	void SetFront(float3 front);
	void SetUp(float3 up);

	const float4x4 GetViewMatrix() const;
	const float4x4 GetProjectionMatrix() const;
	const float4x4 GetOpenGLViewMatrix() const;
	const float4x4 GetOpenGLProjectionMatrix() const;
	
	void MoveFront(const float& speed);
	void MoveBack(const float& speed);
	void MoveRight(const float& speed);
	void MoveLeft(const float& speed);
	void MoveUp(const float& speed);
	void MoveDown(const float& speed);

	void Orbit(const float3& rotate_center, const float& motion_x, const float& motion_y);
	void Rotate(const float& motion_x, const float& motion_y);
	void Focus(const float3& focus_center, const float& distance);
	void Focus(const AABB& aabb);

	void Look(const float3& look_pos);

	Frustum GetFrustum();
	Frustum Camera_frustum;

	

private:
	float	aspect_ratio = 0.0f;
	float   vertical_fov = 0.0f;

	bool	frustum_culling = true;
};

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void LookAt(const float3&Spot);
	void Move(const float3&Movement);
	float* GetViewMatrix();
	float* GetProjectionMatrix();
	void SetAspectRatio(int width, int height);
	void SetAspectRatioGame(int width, int height);

	Camera3D* CreateCamera();
	void DestroyCamera(Camera3D* cam);
	void DestroyAllCameras();
	std::vector<Camera3D*> GetCameras();

	Camera3D* GetEditorCamera();
public:
	bool holdingWindowTab = false;	

public:
	std::vector<Camera3D*> cameras;
 
	Camera3D* editor_camera;
	Camera3D* active_camera = nullptr;
};