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

#pragma once

#include "GameObject.h"
#include "container/FastList.h"
#include "container/List.h"

namespace Viry3D
{
	class Renderer;

	class World
	{
	public:
		static void AddGameObject(const Ref<GameObject>& obj);
		static void AddGameObjects(const FastList<Ref<GameObject>>& objs);
		static void Init();
		static void Deinit();
		static void Update();
		static void OnPause();
		static void OnResume();

	private:
		static void FindAllRenders(const FastList<Ref<GameObject>>& objs, List<Renderer*>& renderers, bool include_inactive, bool include_disable, bool static_only);

	private:
		static FastList<Ref<GameObject>> m_gameobjects;
		static List<Ref<GameObject>> m_gameobjects_start;
		static Mutex m_mutex;
	};
}
