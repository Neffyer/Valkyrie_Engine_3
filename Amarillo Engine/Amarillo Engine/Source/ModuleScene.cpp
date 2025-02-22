#include "ModuleScene.h"
#include "../External/ImGuizmo/ImGuizmo.h"
#include "CFF_Mesh.h"
#include "ModuleJSON.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	app = app;
	ImGuizmo::Enable(true);
}

ModuleScene::~ModuleScene()
{

}

bool ModuleScene::Init()
{
	root_object = new GameObject("Scene");
	game_objects.push_back(root_object);

	GameObject* camera = CreateGameObject("Camera", nullptr);
	camera->AddComponent(ComponentTypes::CAMERA);

	//init loadedScene
	JSON_Doc tmpDoc;

	//tmpDoc = App->json_module->CreateJSON(App->file_system->GetLibraryScenePath().c_str(), "Scene", "ascene");

	tmpDoc.SetNumber3("Editor Camera Pos", App->camera->editor_camera->GetPosition());
	tmpDoc.SetNumber3("Editor Camera PosX (Rigth)", App->camera->editor_camera->GetXDir());
	tmpDoc.SetNumber3("Editor Camera PosY (Up)", App->camera->editor_camera->GetYDir());
	tmpDoc.SetNumber3("Editor Camera PosZ (Front)", App->camera->editor_camera->GetZDir());
	tmpDoc.Save();

	tmpDoc.SetHierarchy("Hierarchy", game_objects);
	
	loadedScene = &tmpDoc;
	loadedScene->Save();

	tmpDoc.CleanUp();

	return true;
}

update_status ModuleScene::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_W) && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Right))
		gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	else if (App->input->GetKey(SDL_SCANCODE_E) && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Right))
		gizmoOperation = ImGuizmo::OPERATION::ROTATE;
	else if (App->input->GetKey(SDL_SCANCODE_R) && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Right))
		gizmoOperation = ImGuizmo::OPERATION::SCALE;

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{

		//App->camera->active_camera->OnClick(App->input->GetMouseX(), App->window->GetWindowSize()[0] - App->input->GetMouseY());
	}

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) 
	{
		LOG("Saved Scene!");
		SaveScene();
	}

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		LOG("Loaded Scene!");
		LoadScene(false);
	}



	UpdateGameObjects();

	//Actua
	
	return update_status();
}

bool ModuleScene::CleanUp()
{

	return false;
}

GameObject* ModuleScene::CreateGameObject(std::string name,GameObject* parent)
{
	GameObject* newObject = new GameObject(name);

	if (parent == nullptr)
	{
		newObject->SetParent(root_object);

	}
	else
	{
		parent->AddChildren(newObject);
	}
	game_objects.push_back(newObject);
	return newObject;
}

void ModuleScene::DeleteGameObject(GameObject* gameobject)
{
	std::vector<GameObject*> todelete;

	todelete.push_back(gameobject);
	gameobject->RemoveParent();

	while (!todelete.empty())
	{
		GameObject* deletes = *todelete.begin();

		todelete.erase(todelete.begin());

		deletes->Disable();

		for (std::vector<GameObject*>::iterator dt = deletes->children.begin(); dt != deletes->children.end(); ++dt)
		{
			todelete.push_back(*dt);
		}
		deletes->children.clear();

		for (size_t i = 0; i < deletes->components.size(); ++i)
		{
			Component* component = deletes->components[i];
			delete deletes->components[i];
		}
		deletes->components.clear();

		if (deletes == App->editor->GameObject_selected)
		{
			App->editor->GameObject_selected = nullptr;
		}
	}
}

GameObject* ModuleScene::LoadMeshAndTexture(std::string path_mesh, std::string path_tex)
{
	GameObject* gameObject = App->mesh->LoadMesh(path_mesh.c_str());

	for (std::vector<GameObject*>::iterator it = gameObject->children.begin(); it != gameObject->children.end(); ++it)
	{
		App->texture->LoadTextureToGameObject((*it), path_tex);
	}
	
	return gameObject;
}

GameObject* ModuleScene::CreateChildObject(std::string name)
{

	GameObject* newObject = new GameObject(name);
	root_object->AddChildren(newObject);
	newObject->SetParent(root_object);
	return newObject;
}

std::vector<GameObject*> ModuleScene::GetGameObjects()
{
	return game_objects;
}

void ModuleScene::ImGuizmoHandling()
{
	ImGuizmo::BeginFrame();
	if (App->editor->GameObject_selected == nullptr) return;

	ComponentTransform* selected_transform = (ComponentTransform*)App->editor->GameObject_selected->GetComponent(ComponentTypes::TRANSFORM);

	float4x4 viewMatrix = App->camera->editor_camera->Camera_frustum.ViewMatrix();
	viewMatrix.Transpose();
	float4x4 projectionMatrix = App->camera->editor_camera->Camera_frustum.ProjectionMatrix();
	projectionMatrix.Transpose();
	float4x4 modelProjection = selected_transform->local_matrix;
	modelProjection.Transpose();

	ImGuizmo::SetRect(App->editor->windowPosition.x, App->editor->windowPosition.y + App->editor->offset, App->editor->size_texture_scene.x, App->editor->size_texture_scene.y);

	//gizmoOperation
	float modelPtr[16];
	memcpy(modelPtr, modelProjection.ptr(), 16 * sizeof(float));

	ImGuizmo::MODE finalMode = (gizmoOperation == ImGuizmo::OPERATION::SCALE ? ImGuizmo::MODE::LOCAL : guizmoMode);

	ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), gizmoOperation, finalMode, modelPtr);


	if (ImGuizmo::IsUsing())
	{
		//Reformat ImGuizmo Transform output to our matrix
		float4x4 newMatrix;
		newMatrix.Set(modelPtr);
		modelProjection = newMatrix.Transposed();

		selected_transform->local_matrix = modelProjection;
		App->editor->GameObject_selected->transform->RecalculateTransformHierarchy();
	}
}

void ModuleScene::UpdateGameObjects()
{
	for (std::vector<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); ++it)
	{
		GameObject* update = *it;
		if (!update->active)
		{
			continue;
		}
		update->Update();
	}
}

void ModuleScene::DebugDrawGameObjects()
{
	for (std::vector<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); ++it)
	{
		GameObject* update = *it;
		if (!update->active)
		{
			continue;
		}
		update->DebugDraw();
	}
}

bool ModuleScene::IsPointInsideAABB(const float3& point, const AABB& aabb)
{
	return point.x >= aabb.minPoint.x && point.x <= aabb.maxPoint.x &&
		point.y >= aabb.minPoint.y && point.y <= aabb.maxPoint.y &&
		point.z >= aabb.minPoint.z && point.z <= aabb.maxPoint.z;
}

void ModuleScene::SaveScene()
{
	JSON_Doc tmpDoc;

	tmpDoc = App->json_module->CreateJSON(App->file_system->GetLibraryScenePath().c_str(), "scene_backup", "ascene");

	tmpDoc.SetNumber3("Editor Camera Pos", App->camera->editor_camera->GetPosition());
	tmpDoc.SetNumber3("Editor Camera PosX (Rigth)", App->camera->editor_camera->GetXDir());
	tmpDoc.SetNumber3("Editor Camera PosY (Up)", App->camera->editor_camera->GetYDir());
	tmpDoc.SetNumber3("Editor Camera PosZ (Front)", App->camera->editor_camera->GetZDir());
	tmpDoc.Save();

	tmpDoc.SetHierarchy("Hierarchy", game_objects);

	//LOG("Camera saved position: %f , %f , %f", App->camera->editor_camera->GetPosition().x, App->camera->editor_camera->GetPosition().y, App->camera->editor_camera->GetPosition().z)

	playScene = &tmpDoc;
	playScene->Save();

	LOG("Camera saved position: %f , %f , %f", playScene->GetNumber3("Editor Camera Pos").x, playScene->GetNumber3("Editor Camera Pos").y, playScene->GetNumber3("Editor Camera Pos").z)

	tmpDoc.CleanUp();
}

void ModuleScene::LoadScene(bool playScene)
{
	if (playScene)
	{
		JSON_Doc* sceneToLoad = loadedScene->GetJSON((const std::string)(App->file_system->GetLibraryScenePath() + "scene_Backup" + ".ascene"));

		//Load Editor Camera Position
		App->camera->editor_camera->SetPosition(sceneToLoad->GetNumber3("Editor Camera Pos"));
		App->camera->editor_camera->SetUp(sceneToLoad->GetNumber3("Editor Camera PosY (Up)"));
		App->camera->editor_camera->SetFront(sceneToLoad->GetNumber3("Editor Camera PosZ (Front)"));
		LOG("Camera loaded position: %f , %f , %f", sceneToLoad->GetNumber3("Editor Camera Pos").x, sceneToLoad->GetNumber3("Editor Camera Pos").y, sceneToLoad->GetNumber3("Editor Camera Pos").z)

		//ClearScene(); -> TODO

		//game_objects = sceneToLoad->GetHierarchy("Hierarchy");
		root_object = game_objects[0];

		delete sceneToLoad;
	}
	else
	{
		std::string asDir = "../Assets/Scenes/";
		JSON_Doc* sceneToLoad = loadedScene->GetJSON((const std::string)(asDir + "Base_Layout" + ".ascene"));

		//Load Editor Camera Position
		App->camera->editor_camera->SetPosition(sceneToLoad->GetNumber3("Editor Camera Pos"));
		App->camera->editor_camera->SetUp(sceneToLoad->GetNumber3("Editor Camera PosY (Up)"));
		App->camera->editor_camera->SetFront(sceneToLoad->GetNumber3("Editor Camera PosZ (Front)"));
		LOG("Camera loaded position: %f , %f , %f", sceneToLoad->GetNumber3("Editor Camera Pos").x, sceneToLoad->GetNumber3("Editor Camera Pos").y, sceneToLoad->GetNumber3("Editor Camera Pos").z)

		//ClearScene(); -> TODO

		//game_objects = sceneToLoad->GetHierarchy("Hierarchy");
		root_object = game_objects[0];

		delete sceneToLoad;
	}
}

void ModuleScene::TestGameObjectSelection(const LineSegment& ray)
{
	std::map<float, GameObject*> game_object_candidates;

	float closest = 0;
	float furthest = 50;
	
	for (std::vector<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); ++it)
	{
		for (uint m = 0; m < (*it)->components.size(); ++m)
		{
			Component* component = (*it)->components[m];

			if (component->type != ComponentTypes::MESH)
			{
				continue;
			}

			ComponentMesh* componentMesh = (ComponentMesh*)component;

			if (ray.Intersects(componentMesh->globalAABB))
			{
				if (ray.Intersects(componentMesh->obb, closest, furthest))
				{
					if (!IsPointInsideAABB(ray.a, componentMesh->globalAABB))
					{
						game_object_candidates[closest] = (*it);
					}
				}

			}
			
		}

	}

	std::vector<GameObject*> game_objects_sorted;

	//Turn map to vector cause iterating vectors is easy :)
	for (std::map<float, GameObject*>::iterator i = game_object_candidates.begin(); i != game_object_candidates.end(); ++i)
	{
		game_objects_sorted.push_back(i->second);
	}

	for (uint i = 0; i < game_objects_sorted.size(); ++i)
	{
		ComponentMesh* mesh_to_test = (ComponentMesh*)game_objects_sorted[i]->GetComponent(ComponentTypes::MESH);
		ModuleMesh::Mesh* mesh = mesh_to_test->mesh_;
		if (mesh != nullptr)
		{
			LineSegment local_ray = ray;

			ComponentTransform* c_transform = (ComponentTransform*)game_objects_sorted[i]->GetComponent(ComponentTypes::TRANSFORM);

			local_ray.Transform(c_transform->GetTransformMatrix().Inverted());

			//Iterate points in a mesh jump 3 by 3 because triangles
			for (uint j = 0; j < mesh->indices.size(); j += 3) {
				uint triangle_indices[3] = { mesh->indices[j], mesh->indices[j + 1], mesh->indices[j + 2] };

				float3 point_a(mesh->ourVertex[triangle_indices[0]].Position.x, mesh->ourVertex[triangle_indices[0]].Position.y, mesh->ourVertex[triangle_indices[0]].Position.z);
				float3 point_b(mesh->ourVertex[triangle_indices[1]].Position.x, mesh->ourVertex[triangle_indices[1]].Position.y, mesh->ourVertex[triangle_indices[1]].Position.z);
				float3 point_c(mesh->ourVertex[triangle_indices[2]].Position.x, mesh->ourVertex[triangle_indices[2]].Position.y, mesh->ourVertex[triangle_indices[2]].Position.z);

				Triangle triangle(point_a, point_b, point_c);

				if (local_ray.Intersects(triangle, nullptr, nullptr)) {
					App->editor->GameObject_selected = game_objects_sorted[i];
					return;
				}
			}



		}
	}
}

