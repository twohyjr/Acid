#pragma once

#include <map>
#include "Maths/Delta.hpp"
#include "Maths/Timer.hpp"
#include "IModule.hpp"

namespace Flounder
{
	/// <summary>
	/// The default updater for the engine.
	/// </summary>
	class F_EXPORT ModuleRegister
	{
	private:
		typedef std::pair<std::string, IModule *> ModulePair;

		std::multimap<float, ModulePair> *m_modules;

		Delta *m_deltaUpdate;
		Delta *m_deltaRender;
		Timer *m_timerUpdate;
		Timer *m_timerRender;
	public:
		/// <summary>
		/// Creates a new module register.
		/// </summary>
		ModuleRegister();

		/// <summary>
		/// Deconstructor for the module register.
		/// </summary>
		~ModuleRegister();

		/// <summary>
		/// Updates all modules in order.
		/// </summary>
		void Update();

		/// <summary>
		/// Registers a module with the register.
		/// </summary>
		/// <param name="update"> The modules update type. </param>
		/// <param name="name"> The modules name. </param>
		/// <param name="T"> The modules type. </param>
		template<typename T>
		void RegisterModule(const ModuleUpdate &update, const std::string &name)
		{
		//	if (m_modules->find(name) != m_modules->end())
		//	{
		//		fprintf(stderr, "Module '%s' is already registered!\n", name.c_str());
		//		return;
		//	}

			T *module = static_cast<T *>(malloc(sizeof(T)));
			float offset = update + (0.01f * static_cast<float>(m_modules->size()));
			m_modules->insert(std::make_pair(offset, std::make_pair(name, module)));
			new(module) T();
		}

		/// <summary>
		/// Gets a module from the register.
		/// </summary>
		/// <param name="name"> The module name to get. </param>
		/// <returns> The module object. </returns>
		IModule *GetModule(const std::string &name);

		/// <summary>
		/// Gets the delta (seconds) between updates.
		/// </summary>
		/// <returns> The delta between updates. </returns>
		float GetDelta() { return m_deltaUpdate->GetChange(); };

		/// <summary>
		/// Gets the delta (seconds) between renders.
		/// </summary>
		/// <returns> The delta between renders. </returns>
		float GetDeltaRender() { return m_deltaRender->GetChange(); };
	private:
		void RunUpdate(const ModuleUpdate &typeUpdate) const;
	};
}