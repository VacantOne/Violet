#include "vlpch.h"
#include "ScriptGlue.h"
#include "ScriptEngine.h"

#include "Violet/Core/UUID.h"
#include "Violet/Core/KeyCode.h"
#include "Violet/Core/Input.h"

#include "Violet/Scene/Scene.h"
#include "Violet/Scene/Entity.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/b2_body.h"
namespace Violet {
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define VL_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Violet.InternalCalls::" #Name, Name)

	// 定义静态C++函数，用于在C++中实现具体逻辑

	// 将MonoString转换为UTF-8字符串并打印输出
	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	// 打印输入的glm::vec3向量并归一化
	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		//VL_CORE_WARN("Value: {0}", *parameter);
		std::cout << parameter->x << "," << parameter->y << "," << parameter->z << std::endl;
		*outResult = glm::normalize(*parameter);
	}

	// 计算输入的glm::vec3向量与自身的点积并返回结果
	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		//VL_CORE_WARN("Value: {0}", *parameter);
		std::cout << parameter->x << "," << parameter->y << "," << parameter->z << std::endl;
		return glm::dot(*parameter, *parameter);
	}

	static MonoObject* GetScriptInstance(UUID entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		VL_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		VL_CORE_ASSERT(entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		VL_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static uint64_t Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);

		Scene* scene = ScriptEngine::GetSceneContext();
		VL_CORE_ASSERT(scene);
		Entity entity = scene->FindEntityByName(nameCStr);
		mono_free(nameCStr);

		if (!entity)
			return 0;

		return entity.GetUUID();
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		VL_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		VL_CORE_ASSERT(entity);

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		VL_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		VL_CORE_ASSERT(entity);

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		VL_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		VL_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		VL_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		VL_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		body->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
			{
				// 获取每个组件的类型名称
				std::string_view typeName = typeid(Component).name();
				// 查找最后一个冒号的位置以提取实际的结构名称
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				// 构造对应C#类的完整名称
				std::string managedTypename = fmt::format("Violet.{}", structName);

				// 从核心程序集中检索托管类型的MonoType*
				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					VL_CORE_ERROR("无法找到组件类型 {}", managedTypename);
					return;
				}
				else
				{
					VL_CORE_TRACE("注册组件类型 {}", managedTypename);
				}
				// 将每个组件类型与一个Lambda函数关联起来，以检查实体是否具有该组件
				s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}

	// 注册函数，将C++函数注册为C#中的内部调用
	void ScriptGlue::RegisterFunctions()
	{
		VL_ADD_INTERNAL_CALL(NativeLog);
		VL_ADD_INTERNAL_CALL(NativeLog_Vector);
		VL_ADD_INTERNAL_CALL(NativeLog_VectorDot);
		VL_ADD_INTERNAL_CALL(GetScriptInstance);

		VL_ADD_INTERNAL_CALL(Entity_HasComponent);
		VL_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		VL_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		VL_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		VL_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		VL_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		VL_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}

}