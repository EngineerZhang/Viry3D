/*
 * Viry3D
 * Copyright 2014-2017 by Stack - stackos@qq.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Main.h"
#include "Application.h"
#include "GameObject.h"
#include "graphics/Camera.h"
#include "graphics/Graphics.h"
#include "graphics/Texture2D.h"
#include "graphics/Mesh.h"
#include "graphics/Material.h"
#include "renderer/MeshRenderer.h"
#include "ios/ARScene.h"

using namespace Viry3D;

class AppAR: public Application
{
public:
    AppAR()
    {
        this->SetName("Viry3D::AppAR");
        this->SetInitSize(1280, 720);
    }
    
    virtual void Start()
    {
        this->CreateFPSUI(20, 1, 1);
        
        auto camera = GameObject::Create("camera")->AddComponent<Camera>();

#if VR_IOS
        camera->GetTransform()->SetPosition(Vector3(0, 6, -10));
        camera->GetTransform()->SetRotation(Quaternion::Euler(30, 0, 0));
        camera->SetCullingMask(1 << 0);
        
        auto mesh = Mesh::Create();
        mesh->vertices.Add(Vector3(-1, 1, 0));
        mesh->vertices.Add(Vector3(-1, -1, 0));
        mesh->vertices.Add(Vector3(1, -1, 0));
        mesh->vertices.Add(Vector3(1, 1, 0));
        mesh->uv.Add(Vector2(0, 1));
        mesh->uv.Add(Vector2(1, 1));
        mesh->uv.Add(Vector2(1, 0));
        mesh->uv.Add(Vector2(0, 0));
        unsigned short triangles[] = {
            0, 1, 2, 0, 2, 3
        };
        mesh->triangles.AddRange(triangles, 6);
        mesh->Update();
        
        auto mat = Material::Create("ARBackgroundYUV");
        
        auto obj = GameObject::Create("mesh");
        obj->SetActive(false);
        
        auto renderer = obj->AddComponent<MeshRenderer>();
        renderer->SetSharedMesh(mesh);
        renderer->SetSharedMaterial(mat);
        
        m_background_mat = mat;
        m_background_obj = obj;
        
        if (ARScene::IsSupported())
        {
            m_ar = RefMake<ARScene>();
            m_ar->RunSession();
        }
#endif
    }
    
#if VR_IOS
    virtual void Update()
    {
        if (m_ar)
        {
            m_ar->UpdateSession();
            
            auto texture_y = m_ar->GetBackgroundTextureY();
            auto texture_uv = m_ar->GetBackgroundTextureUV();
            if (texture_y && texture_uv)
            {
                if (m_background_mat.expired() == false &&
                    m_background_obj.expired() == false)
                {
                    if (m_background_obj.lock()->IsActiveSelf() == false)
                    {
                        m_background_obj.lock()->SetActive(true);
                        
                        this->OnBackgroundRenderStart();
                    }
                    
                    m_background_mat.lock()->SetTexture("_MainTexY", texture_y);
                    m_background_mat.lock()->SetTexture("_MainTexUV", texture_uv);
                };
            }
        }
    }
    
    void OnBackgroundRenderStart()
    {
        
    }
    
    virtual void OnPause()
    {
        Application::OnPause();
        
        if (m_ar)
        {
            m_ar->PauseSession();
        }
    }
    
    virtual void OnResume()
    {
        Application::OnResume();
        
        if (m_ar)
        {
            m_ar->RunSession();
        }
    }
    
    Ref<ARScene> m_ar;
    WeakRef<Material> m_background_mat;
    WeakRef<GameObject> m_background_obj;
#endif
};

#if 1
VR_MAIN(AppAR);
#endif