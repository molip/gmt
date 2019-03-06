#pragma once

#include <memory>

namespace GMT::Model
{
	class Model;
	class Selection;
	using SelectionPtr = std::unique_ptr<Selection>;

	namespace Notification { class Base; }
}

namespace GMT::Model::Command
{
	class Base
	{
	public:
		virtual ~Base();

		class CommandContext
		{
		public:
			CommandContext(Model& model, const Selection& selection);
			~CommandContext();
			
			void Deselect(const Selection& selection);

			Model& GetModel() { return m_model; }
			SelectionPtr HarvestSelection();

		private:
			Model& m_model;
			const Selection& m_selection;
			SelectionPtr m_newSelection;
		};

		virtual bool CanDo() const { return true; }

		virtual void Do(CommandContext& ctx) = 0;
		virtual void Undo(CommandContext& ctx) = 0;
		virtual void Redo(CommandContext& ctx) { Do(ctx); }
		virtual void UndoNoUpdate() {}
	};

	using BasePtr = std::unique_ptr<Base>;
}
