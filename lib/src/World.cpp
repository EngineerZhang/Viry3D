/*
* Viry3D
* Copyright 2014-2018 by Stack - stackos@qq.com
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

#include "World.h"
#include "Resource.h"
#include "Profiler.h"
#include "ui/Font.h"
#include "time/Time.h"
#include "graphics/Shader.h"
#include "graphics/Camera.h"
#include "graphics/RenderTexture.h"
#include "graphics/LightmapSettings.h"
#include "renderer/Renderer.h"
#include "audio/AudioManager.h"
#include "physics/Physics.h"
#include <stdlib.h>

namespace Viry3D
{
	FastList<Ref<GameObject>> World::m_gameobjects;
	List<Ref<GameObject>> World::m_gameobjects_start;
	Mutex World::m_mutex;

	void World::AddGameObject(const Ref<GameObject>& obj)
	{
		m_mutex.lock();
		m_gameobjects_start.AddLast(obj);
		m_mutex.unlock();
	}

	void World::AddGameObjects(const FastList<Ref<GameObject>>& objs)
	{
		m_mutex.lock();
        for (const auto& i : objs)
        {
            m_gameobjects_start.AddLast(i);
        }
		m_mutex.unlock();
	}

	void World::Update()
	{
		Physics::Update();

        for (auto i = m_gameobjects.begin(); i != m_gameobjects.end(); )
        {
            auto& obj = *i;
            if (!obj->m_deleted)
            {
                if (obj->IsActiveInHierarchy())
                {
                    obj->Start();
                    obj->Update();
                }
            }
            else
            {
                i = m_gameobjects.Remove(i);
                continue;
            }

            ++i;
        }

        for (auto i = m_gameobjects.begin(); i != m_gameobjects.end(); )
        {
            auto& obj = *i;
            if (!obj->m_deleted)
            {
                if (obj->IsActiveInHierarchy())
                {
                    obj->LateUpdate();
                }
            }
            else
            {
                i = m_gameobjects.Remove(i);
                continue;
            }

            ++i;
        }

        List<Ref<GameObject>> starts;
        do
        {
            for (auto& i : starts)
            {
                auto& obj = i;
                if (!obj->m_deleted)
                {
                    if (obj->IsActiveInHierarchy())
                    {
                        obj->Start();
                        obj->Update();
                    }
                }
            }

            for (auto& i : starts)
            {
                auto& obj = i;
                if (!obj->m_deleted)
                {
                    if (obj->IsActiveInHierarchy())
                    {
                        obj->LateUpdate();
                    }
                }
            }

            for (auto& i : starts)
            {
                m_gameobjects.AddLast(i);
            }

            m_mutex.lock();
            starts = m_gameobjects_start;
            m_gameobjects_start.Clear();
            m_mutex.unlock();
        } while (starts.Size() > 0);

		if (Renderer::IsRenderersDirty())
		{
			Renderer::SetRenderersDirty(false);
			Renderer::ClearPasses();

			List<Renderer*>& renderers = Renderer::GetRenderers();
			renderers.Clear();

			FindAllRenders(m_gameobjects, renderers, false, false, false);
		}
	}

	void World::FindAllRenders(const FastList<Ref<GameObject>>& objs, List<Renderer*>& renderers, bool include_inactive, bool include_disable, bool static_only)
	{
        for (auto& i : objs)
        {
            if (!i->m_deleted)
            {
                if (include_inactive || i->IsActiveInHierarchy())
                {
                    if (!static_only || i->IsStatic())
                    {
                        auto rs = i->GetComponents<Renderer>();
                        for (auto& j : rs)
                        {
                            if (include_disable || j->IsEnable())
                            {
                                renderers.AddLast(j.get());
                            }
                        }
                    }
                }
            }
        }
	}

	void World::OnPause()
	{
		AudioManager::OnPause();
	}

	void World::OnResume()
	{
		AudioManager::OnResume();
	}

	void World::Init()
	{
		srand((unsigned int) Time::GetTimeMS());

		Component::RegisterComponents();

		Font::Init();
		Shader::Init();
		Object::Init();
		Camera::Init();
		RenderTexture::Init();
		AudioManager::Init();
		Renderer::Init();
		Physics::Init();
		Resource::Init();
	}

	void World::Deinit()
	{
		LightmapSettings::Clear();
		Resource::Deinit();
		m_gameobjects.Clear();

        m_mutex.lock();
        m_gameobjects_start.Clear();
        m_mutex.unlock();

		Physics::Deinit();
		Renderer::Deinit();
		AudioManager::Deinit();
		RenderTexture::Deinit();
		Camera::Deinit();
		Object::Deinit();
		Shader::Deinit();
		Font::Deinit();
	}
}
