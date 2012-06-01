#include "CodaScriptCommands-Reference.h"

#define PI	3.151592653589793

namespace ConstructionSetExtender
{
	namespace BGSEEScript
	{
		namespace Commands
		{
			namespace Reference
			{
				CodaScriptCommandRegistrarDef("Reference")

				CodaScriptCommandPrototypeDef(CreateRef);
				CodaScriptCommandPrototypeDef(GetRefPosition);
				CodaScriptCommandPrototypeDef(GetRefRotation);
				CodaScriptCommandPrototypeDef(GetRefScale);
				CodaScriptCommandPrototypeDef(GetRefPersistent);
				CodaScriptCommandPrototypeDef(GetRefDisabled);
				CodaScriptCommandPrototypeDef(GetRefVWD);
				CodaScriptCommandPrototypeDef(GetRefBaseForm);
				CodaScriptCommandPrototypeDef(SetRefRotation);
				CodaScriptCommandPrototypeDef(SetRefPosition);
				CodaScriptCommandPrototypeDef(SetRefScale);
				CodaScriptCommandPrototypeDef(SetRefPersistent);
				CodaScriptCommandPrototypeDef(SetRefDisabled);
				CodaScriptCommandPrototypeDef(SetRefVWD);
				CodaScriptCommandPrototypeDef(GetCellObjects);
				CodaScriptCommandPrototypeDef(GetCurrentRenderWindowSelection);

				CodaScriptCommandParamData(CreateRef, 9)
				{
					{ "Base Form", ICodaScriptDataStore::kDataType_Reference },
					{ "Pos X", ICodaScriptDataStore::kDataType_Numeric },
					{ "Pos Y", ICodaScriptDataStore::kDataType_Numeric },
					{ "Pos X", ICodaScriptDataStore::kDataType_Numeric },
					{ "Rot X", ICodaScriptDataStore::kDataType_Numeric },
					{ "Rot Y", ICodaScriptDataStore::kDataType_Numeric },
					{ "Rot Z", ICodaScriptDataStore::kDataType_Numeric },
					{ "Cell", ICodaScriptDataStore::kDataType_Reference },
					{ "Worldspace", ICodaScriptDataStore::kDataType_Reference }
				};

				CodaScriptCommandParamData(GetRefPosition, 2)
				{
					{ "Reference", ICodaScriptDataStore::kDataType_Reference },
					{ "Axis", ICodaScriptDataStore::kDataType_String }
				};

				CodaScriptCommandParamData(GetRefRotation, 2)
				{
					{ "Reference", ICodaScriptDataStore::kDataType_Reference },
					{ "Axis", ICodaScriptDataStore::kDataType_String }
				};

				CodaScriptCommandParamData(SetRefPosition, 3)
				{
					{ "Reference", ICodaScriptDataStore::kDataType_Reference },
					{ "Axis", ICodaScriptDataStore::kDataType_String },
					{ "Value", ICodaScriptDataStore::kDataType_Numeric }
				};

				CodaScriptCommandParamData(SetRefRotation, 3)
				{
					{ "Reference", ICodaScriptDataStore::kDataType_Reference },
					{ "Axis", ICodaScriptDataStore::kDataType_String },
					{ "Value", ICodaScriptDataStore::kDataType_Numeric }
				};

				CodaScriptCommandParamData(GetCellObjects, 1)
				{
					{ "Cell", ICodaScriptDataStore::kDataType_Reference }
				};

				CodaScriptCommandHandler(CreateRef)
				{
					TESForm* BaseForm = NULL;
					TESForm* Cell = NULL;
					TESForm* WorldSpace = NULL;
					Vector3 Position, Rotation;

					CodaScriptCommandExtractArgs(&BaseForm, &Position.x, &Position.y, &Position.z, &Rotation.x, &Rotation.y, &Rotation.z, &Cell, &WorldSpace);
					ExtractFormArguments(3, &BaseForm, &Cell, &WorldSpace);

					if (!BaseForm || !Cell)
						return false;

					TESObject* Base = CS_CAST(BaseForm, TESForm, TESObject);
					TESObjectCELL* ParentCell = CS_CAST(Cell, TESForm, TESObjectCELL);
					TESWorldSpace* ParentWorldspace = CS_CAST(WorldSpace, TESForm, TESWorldSpace);

					if (!Base || !ParentCell || (ParentCell->GetIsInterior() == false && !ParentWorldspace))
						return false;

					TESObjectREFR* NewRef = _DATAHANDLER->PlaceObjectRef(Base, &Position, &Rotation, ParentCell, ParentWorldspace, NULL);
					if (NewRef)
						Result->SetFormID(NewRef->formID);
					else
						Result->SetFormID(0);

					return true;
				}

				CodaScriptCommandHandler(GetRefPosition)
				{
					TESForm* Form = NULL;
					CodaScriptStringParameterTypeT Buffer = NULL;

					CodaScriptCommandExtractArgs(&Form, &Buffer);
					ExtractFormArguments(1, &Form);

					if (!Form)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					if (!_stricmp(Buffer, "x"))
						Result->SetNumber(Reference->position.x);
					else if (!_stricmp(Buffer, "y"))
						Result->SetNumber(Reference->position.y);
					else
						Result->SetNumber(Reference->position.z);

					return true;
				}

				CodaScriptCommandHandler(GetRefRotation)
				{
					TESForm* Form = NULL;
					CodaScriptStringParameterTypeT Buffer = NULL;

					CodaScriptCommandExtractArgs(&Form, &Buffer);
					ExtractFormArguments(1, &Form);

					if (!Form)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					if (!_stricmp(Buffer, "x"))
						Result->SetNumber(Reference->rotation.x * 180 / PI);
					else if (!_stricmp(Buffer, "y"))
						Result->SetNumber(Reference->rotation.y * 180 / PI);
					else
						Result->SetNumber(Reference->rotation.z * 180 / PI);

					return true;
				}

				CodaScriptCommandHandler(GetRefScale)
				{
					TESForm* Form = NULL;

					CodaScriptCommandExtractArgs(&Form);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					Result->SetNumber(Reference->scale);
					return true;
				}

				CodaScriptCommandHandler(GetRefPersistent)
				{
					TESForm* Form = NULL;

					CodaScriptCommandExtractArgs(&Form);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					Result->SetNumber((Reference->formFlags & TESForm::kFormFlags_QuestItem));
					return true;
				}

				CodaScriptCommandHandler(GetRefDisabled)
				{
					TESForm* Form = NULL;

					CodaScriptCommandExtractArgs(&Form);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					Result->SetNumber((Reference->formFlags & TESForm::kFormFlags_Disabled));
					return true;
				}

				CodaScriptCommandHandler(GetRefVWD)
				{
					TESForm* Form = NULL;

					CodaScriptCommandExtractArgs(&Form);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					Result->SetNumber((Reference->formFlags & TESForm::kFormFlags_VisibleWhenDistant));
					return true;
				}

				CodaScriptCommandHandler(GetRefBaseForm)
				{
					TESForm* Form = NULL;

					CodaScriptCommandExtractArgs(&Form);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					if (Reference->baseForm)
						Result->SetFormID(Reference->baseForm->formID);
					else
						Result->SetFormID(0);

					return true;
				}

				CodaScriptCommandHandler(SetRefPosition)
				{
					TESForm* Form = NULL;
					CodaScriptStringParameterTypeT Buffer = NULL;
					CodaScriptNumericDataTypeT Value = 0.0;

					CodaScriptCommandExtractArgs(&Form, &Buffer, &Value);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					if (!_stricmp(Buffer, "x"))
						Reference->position.x = Value;
					else if (!_stricmp(Buffer, "y"))
						Reference->position.y = Value;
					else
						Reference->position.z = Value;

					Reference->UpdateNiNode();
					return true;
				}

				CodaScriptCommandHandler(SetRefRotation)
				{
					TESForm* Form = NULL;
					CodaScriptStringParameterTypeT Buffer = NULL;
					CodaScriptNumericDataTypeT Value = 0.0;

					CodaScriptCommandExtractArgs(&Form, &Buffer, &Value);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					if (!_stricmp(Buffer, "x"))
						Reference->position.x = Value * PI / 180;
					else if (!_stricmp(Buffer, "y"))
						Reference->position.y = Value * PI / 180;
					else
						Reference->position.z = Value * PI / 180;

					Reference->UpdateNiNode();
					return true;
				}

				CodaScriptCommandHandler(SetRefScale)
				{
					TESForm* Form = NULL;
					CodaScriptNumericDataTypeT Value = 0.0;

					CodaScriptCommandExtractArgs(&Form, &Value);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					Reference->scale = Value;

					Reference->UpdateNiNode();
					return true;
				}

				CodaScriptCommandHandler(SetRefPersistent)
				{
					TESForm* Form = NULL;
					CodaScriptNumericDataTypeT Value = 0.0;

					CodaScriptCommandExtractArgs(&Form, &Value);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					SME::MiscGunk::ToggleFlag(&Reference->formFlags, TESForm::kFormFlags_QuestItem, (bool)Value);
					return true;
				}

				CodaScriptCommandHandler(SetRefDisabled)
				{
					TESForm* Form = NULL;
					CodaScriptNumericDataTypeT Value = 0.0;

					CodaScriptCommandExtractArgs(&Form, &Value);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					SME::MiscGunk::ToggleFlag(&Reference->formFlags, TESForm::kFormFlags_Disabled, (bool)Value);
					return true;
				}

				CodaScriptCommandHandler(SetRefVWD)
				{
					TESForm* Form = NULL;
					CodaScriptNumericDataTypeT Value = 0.0;

					CodaScriptCommandExtractArgs(&Form, &Value);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectREFR* Reference = CS_CAST(Form, TESForm, TESObjectREFR);
					if (!Reference)
						return false;

					SME::MiscGunk::ToggleFlag(&Reference->formFlags, TESForm::kFormFlags_VisibleWhenDistant, (bool)Value);
					return true;
				}

				CodaScriptCommandHandler(GetCellObjects)
				{
					TESForm* Form = NULL;

					CodaScriptCommandExtractArgs(&Form);
					ExtractFormArguments(1, &Form);

					if (Form == NULL)
						return false;

					TESObjectCELL* Cell = CS_CAST(Form, TESForm, TESObjectCELL);
					if (!Cell)
						return false;

					ICodaScriptDataStore* CellObjects = Utilities->ArrayAllocate(Cell->objectList.Count());
					SME_ASSERT(CellObjects);

					for (TESObjectCELL::ObjectREFRList::Iterator Itr = Cell->objectList.Begin(); !Itr.End() && Itr.Get(); ++Itr)
						Utilities->ArrayPushback(CellObjects, (CodaScriptReferenceDataTypeT)Itr.Get()->formID);

					Result->SetArray(CellObjects);

					return true;
				}

				CodaScriptCommandHandler(GetCurrentRenderWindowSelection)
				{
					ICodaScriptDataStore* Array = Utilities->ArrayAllocate(_RENDERSEL->selectionCount);
					SME_ASSERT(Array);

					for (TESRenderSelection::SelectedObjectsEntry* Itr = _RENDERSEL->selectionList; Itr && Itr->Data; Itr = Itr->Next)
					{
						TESObjectREFR* Ref = CS_CAST(Itr->Data, TESForm, TESObjectREFR);

						if (Ref)
							Utilities->ArrayPushback(Array, (CodaScriptReferenceDataTypeT)Ref->formID);
					}

					Result->SetArray(Array);

					return true;
				}
			}
		}
	}
}
