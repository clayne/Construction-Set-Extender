#pragma once

#include "TESPathGrid.h"
#include "TESDialog.h"

//	EditorAPI: Render control related classes.

class	TESForm;
class	TESObjectREFR;
class	Sky;
class	BSTreeNode;
class	BSTreeModel;

// 4
class Renderer
{
public:
	// members
	//*00*/ void**					vtbl;

	// abstract base class
	virtual void					Render(NiCamera* Camera = nullptr, NiNode* NodeToRender = nullptr, BSRenderedTexture* RenderToTexture = nullptr) = 0;
};
STATIC_ASSERT(sizeof(Renderer) == 0x04);

// 14
class NiWindow : public Renderer
{
public:
	typedef tList<NiPointer<NiCamera>>		RenderCameraListT;

	// members
	//     /*00*/ Renderer
	/*04*/ NiNode*					sceneRootNode;			// smart pointer
	/*08*/ UInt32					renderedFrameCount;		// incremented in Render()
	/*0C*/ RenderCameraListT		attachedCameras;

	// methods
	virtual void					SetSceneNode(NiNode* Node) = 0;
	virtual NiCamera*				AddCamera(NiCamera** OutCamera) = 0;
	virtual bool					HandleResize(void) = 0;
	virtual bool					UpdateCameraFrustum(NiCamera* Camera, int Width, int Height, float ClipDistance) = 0;
};
STATIC_ASSERT(sizeof(NiWindow) == 0x14);

// container class for the editor's renderer implementation
class TESRender
{
public:
	// arbitrarily named
	// 0C
	class PrimaryRenderer
	{
	public:
		// members
		/*00*/ NiWindow*					niWindow;
		/*04*/ NiNode*						primaryCameraParentNode;
		/*08*/ NiCamera*					primaryCamera;

		// methods
		void								RenderNode(NiCamera* Camera = nullptr, NiNode* NodeToRender = nullptr, BSRenderedTexture* RenderToTexture = nullptr);
		void								GetCameraPivot(Vector3* OutPivot, float ScaleFactor);
		const NiVector3*					GetCameraWorldTranslate();
		void								UpdateRendererCameraData();

		void								MoveReferenceSelection(int XOffset, int YOffset, bool AxisX, bool AxisY, bool AxisZ);
		void								RotateReferenceSelection(int Offset, bool AxisX, bool AxisY, bool AxisZ);
		void								ScaleReferenceSelection(int Offset, bool Global);

		static PrimaryRenderer**			Singleton;
	};
	STATIC_ASSERT(sizeof(PrimaryRenderer) == 0x0C);

	enum
	{
		kNodeUpdate_Unk00 =		0,		// translate X axis?
		kNodeUpdate_Unk01,				// translate Y axis?
		kNodeUpdate_Unk02,				// translate Z axis?
		kNodeUpdate_Unk03,
		kNodeUpdate_Unk04,				// zoom?
		kNodeUpdate_Unk05,
		kNodeUpdate_Unk06,
		kNodeUpdate_Unk07,
		kNodeUpdate_Unk08,
		kNodeUpdate_Unk09,
		kNodeUpdate_Unk10,
		kNodeUpdate_Unk11,
	};

	// 30
	struct PickData
	{
		// actually NiPick::Record
		// 44
		struct Record
		{
			// members
			/*00*/ NiAVObject*				picked;
			/*04*/ NiRefObject*				unk04;
			/*08*/ Vector3					intersectionPoint;
			/*14*/ UInt32					unk14[(0x44 - 0x14) >> 2];
		};

		// members
		/*00*/ UInt32						unk00;
		/*04*/ UInt32						unk04;
		/*08*/ UInt32						unk08;				// init to 1
		/*0C*/ UInt32						unk0C;				// init to 1
		/*10*/ UInt8						unk10;				// init to 1
		/*11*/ UInt8						ignoreCulled;
		/*12*/ UInt16						pad12;
		/*14*/ NiNode*						root;				// smart ptr, picking is done on its children
		/*18*/ NiTArray<Record*>			pickRecords;		// NiTArray<NiPick::Record*>
		/*28*/ Record*						unk28;
		/*2C*/ UInt8						unk2C;
		/*2D*/ UInt8						unk2D;
		/*2E*/ UInt8						unk2E;
		/*2F*/ UInt8						unk2F;

		// methods
		void								SetRoot(NiNode* To);
		bool								PerformPick(Vector3* RayOrigin, Vector3* RayDir, bool KeepExisting = false);
	};
	STATIC_ASSERT(sizeof(PickData) == 0x30);
	STATIC_ASSERT(sizeof(PickData::Record) == 0x44);

	// 20
	struct Scenegraph
	{
		// members
		/*00*/ ShadowSceneNode*				sceneRoot;
		/*04*/ NiNode*						objectRoot;
		/*08*/ NiNode*						lodRoot;
		/*0C*/ NiZBufferProperty*			zBuffer;
		/*10*/ NiWireframeProperty*			wireFrame;
		/*14*/ PickData*					pickData;			// used when picking refs
		/*18*/ PickData*					pickData2;			// used when picking land data
		/*1C*/ Sky*							sky;

		static Scenegraph**					Singleton;			// there are actually 2 pointers to this struct, one global (main window) and the other owned by the render window
																// this pointer points to the former
	};
	STATIC_ASSERT(sizeof(Scenegraph) == 0x20);

	enum
	{
		kRenderPassFlags_Ambient		= 1 << 0,
		kRenderPassFlags_Diffuse		= 1 << 1,
		kRenderPassFlags_Texture		= 1 << 2,
		kRenderPassFlags_Specular		= 1 << 3,
	};

	enum
	{
		kDebugRenderPass_NoOfLights		= 1,
		kDebugRenderPass_NoOfPasses		= 2,
		kDebugRenderPass_Overdraw		= 4,
		kDebugRenderPass_Unk05			= 5,				// set when rendering shadows
		kDebugRenderPass_Unk06			= 6,				// set when rendering local maps
	};

	// methods
	static bool								TransformNode(NiAVObject* Node, UInt32 UpdateType, float Multiplier, bool UpdateOnSuccess = true);
	static void								RotateNode(NiAVObject* Node, Vector3* Pivot, int XOffset, int YOffset, float SpeedMultiplier);
	static void								SetCameraFOV(NiCamera* Camera, float FOV, float Width = -1, float Height = -1);		// horizontal FOV
	static TESObjectREFR*					PickRefAtCoords(int X, int Y);
	static TESPathGridPoint*				PickPathGridPointAtCoords(int X, int Y);
	static NiAVObject*						CastRay(PickData* PickBuffer, NiCamera* Camera, int ScreenCoordX, int ScreenCoordY, Vector3* IntersectionPoint, bool ReturnGeometry);

	// misc methods that belong elsewhere
	static void								UpdateAVObject(NiAVObject* Object);		// NiAVObject method
	static void								UpdateDynamicEffectState(NiAVObject* Object);
	static NiNode*							CreateNiNode();
	static void								DeleteNiRefObject(NiRefObject* Object);
	static void								AddToNiNode(NiNode* To, NiAVObject* Child);
	static bool								RemoveFromNiNode(NiNode* From, NiAVObject* Child);	// decrements the child's ref count and deletes if zero (returns true if deleted)
	static ShadowSceneNode*					GetSceneGraphRoot();
	static void								AddProperty(NiAVObject* To, NiProperty* Property, bool InitializeState = true);
	static NiProperty*						GetProperty(NiAVObject* In, UInt16 ID);
	static NiExtraData*						GetExtraData(NiAVObject* In, const char* Name);
	static void								AddExtraData(NiAVObject* To, NiExtraData* ExtraData);
	static void								SetExtraDataName(NiExtraData* To, const char* Name);
	static NiProperty*						CreateProperty(UInt8 Type);		// increments ref count
	static NiSourceTexture*					CreateSourceTexture(const char* FilePath);
	static NiCamera*						CreateCamera();
	static NiTexturingProperty*				CreateTexturingProperty(const char* TexturePath);	// increments ref count
	static void								SetBSShaderPPLightingPropertyDiffuseTexture(BSShaderPPLightingProperty* Property, NiTexture* Texture, UInt8 Index);
	static bool								WindowPointToRay(int X, int Y, Vector3* OutOrigin, Vector3* OutDirection);
	static bool								WindowPointToRay(NiCamera* Camera, int X, int Y, Vector3* OutOrigin, Vector3* OutDirection);


	static NiDX9Renderer**					NiRendererSingleton;
	static NiNode**							PathGridSceneRoot;
	static SInt32*							CurrentRenderPassID;
	static BSShaderProperty::RenderPass**	CurrentRenderPassData;
	static UInt32*							GlobalRenderPassFlags;
	static UInt32*							GlobalRenderPassFlagsBackup;
};

#define _NIRENDERER				(*TESRender::NiRendererSingleton)
#define _PRIMARYRENDERER		(*TESRender::PrimaryRenderer::Singleton)

// container class, arbitrarily named
class TESRenderWindow
{
public:
	enum
	{
		kRenderWindowState_SnapToGrid					= 0x1,
		kRenderWindowState_SnapToAngle					= 0x2,
		kRenderWindowState_AllowRenderWindowCellLoads	= 0x4,
		kRenderWindowState_ShowMarkers					= 0x8,
		kRenderWindowState_SkipInitialCellLoad			= 0x10,
		kRenderWindowState_UseCSDiscAsSource			= 0x20,		// deprecated, probably from the TES3 CS codebase
		kRenderWindowState_UseWorld						= 0x40,
	};

	// 08
	class UndoStack
	{
	public:
		enum
		{
			kUndoOperation_RefCreate	=	1,
			kUndoOperation_RefDelete,
			kUndoOperation_RefChange3D,
			kUndoOperation_Unk04,					// 4-6 used to record landscape changes
			kUndoOperation_Unk05,					// color land?
			kUndoOperation_Unk06					// paint land?
		};

		// 50
		struct UndoData
		{
			/*00*/ UInt32				selIndex;						// index of the ref in its parent selection
			/*04*/ UInt32				selCount;						// number of refs in the selection of which this ref was a part
			/*08*/ TESObjectREFR*		refr;
			/*0C*/ UInt32				operationType;
			/*10*/ Vector3				rotation;
			/*1C*/ Vector3				position;
			/*28*/ float				scale;
			/*2C*/ UInt32				unk2C[(0x48 - 0x2C) >> 2];		// landscape change related
			/*48*/ UndoData*			previous;
			/*4C*/ UndoData*			next;
		};

		// members
		/*00*/ UndoData*				unk00;							// initialized in c'tor, used as a bookend
		/*04*/ UndoData*				first;

		// methods
		void							RecordReference(UInt32 Operation, TESRenderSelection::SelectedObjectsEntry* Selection);
		void							Clear(void);
	};
	STATIC_ASSERT(sizeof(UndoStack) == 0x08);

	// 60
	class RubberBandSelection
	{
	public:
		/*00*/ int							originXCoord;			// set at the start of a drag op, i.e., inside the WM_LBUTTONDOWN handler
		/*04*/ int							originYCoord;
		/*08*/ Vector3						unk08;
		/*14*/ Vector3						bandVertices[4];
		/*44*/ NiVector3*					vertexBuffer;			// 4x dynamic array, passed to NiLines ctor
		/*48*/ NiNode*						sceneNode;
		/*4C*/ NiCamera*					sceneCamera;
		/*50*/ NiLines*						bandBox;				// smart ptr
		/*54*/ float						unk54;					// init to 60.0f, some sort of multiplier used when updating the band box's vertices
		/*58*/ TESRenderSelection*			selectionBuffer;		// allocated by ctor
		/*5C*/ UInt8						dragging;				// set to 1 during selection operations
		/*5D*/ UInt8						hasSelection;			// set to 0 if the area of the band box is zero, i.e., if one of the current mouse coords is the same as that of the origin
		/*5E*/ UInt16						pad5E;
	};
	STATIC_ASSERT(sizeof(RubberBandSelection) == 0x60);

	// methods
	static void							Reset();
	static void							Redraw(bool RefreshPathGrid = false);
	static void							Refresh3D(bool Immediate);
	static void							TogglePathGridEditMode();
	static bool							IsAnyCellLoaded();

	typedef std::function<bool(TESObjectREFR*)>		CellObjectListVisitorT;
	static UInt32						GetActiveCellObjects(TESObjectREFRArrayT& OutList, CellObjectListVisitorT Visitor);		// enumerates refs in the current interior/exterior grid and returns the count
	static bool							GetCellInActiveGrid(TESObjectCELL* Cell);	// returns true if the cell is loaded/visible in the render window
	static std::string					GetCellGeomDescription(TESObjectCELL* Cell);
	static Vector3*						CalculatePathGridPointPositionVectorSum(Vector3& OutPosVecSum);
	static void							PlaceRefAtMousePos(TESObjectREFR* Ref, int X, int Y, bool ResetCurrentSelection = true);
	static void							PlaceRefAtMousePos(TESObjectREFR* Ref, int X, int Y, const Vector3& PositionOffset, const Vector3& Rotation, bool ResetCurrentSelection = true);
	static void							MoveRefToNextZCollisionPlane(TESObjectREFR* Ref);

	static HWND*						WindowHandle;
	static int*							ScreenWidth;
	static int*							ScreenHeight;
	static TESRenderSelection**			ClipboardSelection;
	static UndoStack**					UndoBuffer;
	static RubberBandSelection**		RubberBandSelector;
	static TESRender::PickData*			PickBuffer;

	static UInt32*						StateFlags;
	static UInt8*						NonLandObjectsHiddenFlag;
	static UInt8*						LandscapeHiddenFlag;
	static UInt8*						WaterHiddenFlag;
	static UInt8*						FullBrightLightingFlag;
	static UInt8*						SkyFlag;
	static UInt8*						LightRadiusFlag;

	static UInt8*						RefreshFlag;
	static UInt8*						PathGridEditFlag;
	static UInt8*						LandscapeEditFlag;
	static UInt8*						DraggingSelection;
	static UInt8*						RotatingSelection;
	static UInt8*						CenteringCamera;

	static UInt8*						KeyState_Shift;
	static UInt8*						KeyState_Control;
	static UInt8*						KeyState_SpaceMMB;
	static UInt8*						KeyState_X;
	static UInt8*						KeyState_Y;
	static UInt8*						KeyState_Z;
	static UInt8*						KeyState_S;
	static UInt8*						KeyState_V;
	static POINT*						LastMouseCoords;

	static TESObjectCELL**				ActiveCell;						// points to the current interior cell or the exterior cell at the camera's position/current selection
	static TESLandTexture**				ActiveLandscapeTexture;
	static TESObjectREFR**				SnapReference;

	static float*						RefMovementSpeed;
	static float*						RefRotationSpeed;
	static UInt32*						SnapAngle;
	static UInt32*						SnapGridDistance;
	static float*						CameraRotationSpeed;
	static float*						CameraZoomSpeed;
	static float*						CameraPanSpeed;

	static PathGridPointListT*			SelectedPathGridPoints;

	static HCURSOR*						CursorArrow;
	static HCURSOR*						CursorSelect;
	static HCURSOR*						CursorMove;

	enum
	{
		kTimer_ViewportUpdate = 1,
	};
};

#define _RENDERUNDO				(*TESRenderWindow::UndoBuffer)

// arbitrarily named
// used in the runtime as well, IIRC
// 20
class TESSceneNodeDebugData
{
public:
	// members
	/*00*/ HINSTANCE				instance;
	/*04*/ HWND						parent;
	/*08*/ HWND						debugWindow;
	/*0C*/ HWND						treeView;
	/*10*/ HTREEITEM				rootTreeItem;
	/*14*/ HIMAGELIST				imageList;
	/*18*/ NiNode*					sceneNode;
	/*1C*/ char*					windowTitle;		// allocated by the ctor

	// methods
	TESSceneNodeDebugData*			Initialize(HINSTANCE Instance, HWND Parent, NiNode* Node, const char* WindowTitle, int X, int Y, int Width, int Height);
};
STATIC_ASSERT(sizeof(TESSceneNodeDebugData) == 0x20);

// preview controls used in various dialogs
// 48
class TESRenderControl
{
public:
	// passed to the ctor
	// 0C
	struct Parameters
	{
		/*00*/ int				previewOutputCtrlID;		// button
		/*04*/ float			renderTargetWidth;
		/*08*/ float			renderTargetHeight;
	};

	enum { kPreviewOutputCtrlID = 2175 };

	// members
	//*00*/ void**					vtbl;
	/*04*/ int						outputCtrlID;
	/*08*/ float					renderTargetWidth;
	/*0C*/ float					renderTargetHeight;
	/*10*/ HWND						parentDialog;
	/*14*/ NiNode*					sceneRoot;		// smart ptr
	/*18*/ NiDirectionalLight*		sceneLight;		// smart ptr
	/*1C*/ NiNode*					cameraNode;		// smart ptr
	/*20*/ NiCamera*				camera;			// smart ptr
	/*24*/ NiWindow*				renderer;
	/*28*/ UInt32					unk28;
	/*2C*/ BSRenderedTexture*		unk2C;
	/*30*/ UInt32					unk30;
	/*34*/ UInt32					unk34;
	/*38*/ UInt32					unk38;
	/*3C*/ TESSceneNodeDebugData*	debugData;
	/*40*/ int						currentMouseXCoord;
	/*44*/ int						currentMouseYCoord;

	// methods
	virtual void				Update(void) = 0;
	virtual void				Render(void) = 0;
	virtual void				Reset(void) = 0;
	virtual void				HandleDeviceReset(void) = 0;
	virtual void				Dtor(bool ReleaseMemory) = 0;
	virtual void				AddPreviewNode(NiNode* Node) = 0;			// adds the node to the scene root and increments its ref count
	virtual void				RemovePreviewNode(NiNode* Node) = 0;		// removes the node from the scene root and decrements its ref count
	virtual void				ResetCamera(void) = 0;
	virtual void				HandleResize(void) = 0;
	virtual void				Present(float Time = 0.f) = 0;
	virtual LRESULT				DialogMessageCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lPARAM, LONG_PTR OutUnk05) = 0;		// OutUnk05 set to 1 inside the WM_DRAWITEM callback, when in the method call Render()
};
STATIC_ASSERT(sizeof(TESRenderControl) == 0x48);

// 68
class TESPreviewControl : public TESRenderControl
{
public:
	// members
	//     /*00*/ TESRenderControl
	/*48*/ UInt8					timerEnabled;			// set to 1 when the preview control is primed to handle WM_TIMER messages
	/*49*/ UInt8					pad49[3];
	/*4C*/ SInt32					elapsedTicks;			// set inside the message callback's WM_TIMER handler
	/*50*/ float					remainingTime;			// in seconds, used when previewing animations/facegen (primarily used to play a sound file after a certain amount of time), set to -1 otherwise
	/*54*/ BSString					soundFileName;			// name/path of the sound file that's played once an animation has completed its cycle
	/*5C*/ UInt8					playbackCompleted;		// set to 1 on playback completion
	/*5D*/ UInt8					pad5D[3];
	/*60*/ NiNode*					groundPlaneNode;		// smart ptr
	/*64*/ ShadowSceneNode*			previewSceneRoot;		// smart ptr, shared by all preview controls, destroyed with the last one and created with the first

	// methods
	void							RemovePreviewNode(NiNode* Node);	// this override appears to be bugged in the editor code as it calls AddPreviewNode
																		// so manually call the correct TESRenderControl method
																		// could have something to do with the vanilla ModelLoader NIF leakage
	void							RemoveGroundPlane();

	static TESPreviewControl*		CreatePreviewControl(HWND Dialog, const TESRenderControl::Parameters* Params);
	static void						UpdatePreviewWindows(bool RefreshRenderWindow = true);

	typedef tList<TESPreviewControl>	PreviewControlListT;

	static PreviewControlListT*			ActivePreviewControls;
};
STATIC_ASSERT(sizeof(TESPreviewControl) == 0x68);


// arbitrarily named
// 28
class BSTreeManager
{
public:
	typedef cseOverride::NiTPointerMap<TESObjectTREE*, NiPointer<BSTreeModel>*>	TreeObjModelMapT;
	typedef LockFreeMap<BSTreeNode*>	TreeRefNodeMapT;		// key = TESObjectREFR*

	// members
	/*00*/ TreeObjModelMapT*		treeModelMap;
	/*04*/ UInt32					unk04;
	/*08*/ NiZBufferProperty*		zBuffer;		// smart ptr
	/*0C*/ NiMaterialProperty*		material;		// smart ptr
	/*10*/ NiVertexColorProperty*	vertexColor;	// smart ptr
	/*14*/ NiAlphaProperty*			alpha;
	/*18*/ BSXFlags*				bsxFlags;
	/*1C*/ float					unk1C;
	/*20*/ UInt8					drawTrees;
	/*21*/ UInt8					drawLeaves;
	/*22*/ UInt8					unk22;
	/*23*/ UInt8					forceFullLOD;
	/*24*/ TreeRefNodeMapT*			treeRefMap;

	static BSTreeManager**			Singleton;
};
STATIC_ASSERT(sizeof(BSTreeManager) == 0x28);


class Atmosphere;
class Stars;
class Moon;
class Clouds;
class Precipitation;

// 08
class SkyObject
{
public:
	// members
	//*00*/ void**					vtbl;
	/*04*/ NiNode*					rootNode;		// smart ptr

	virtual void Destroy(bool Free) = 0;
};
STATIC_ASSERT(sizeof(SkyObject) == 0x8);


//28
class Sun : public SkyObject
{
public:
	// members
	//*00*/ SkyObject
	/*08*/ NiBillboardNode*		sunBillboard;		// smart ptr
	/*0C*/ NiBillboardNode*		sunGlareBillboard;	// smart ptr
	/*10*/ NiTriShape*			sunGeometry;		// smart ptr
	/*14*/ NiTriShape*			sunGlareGeometry;	// smart ptr
	/*18*/ TESRender::PickData*	pickData;
	/*1C*/ NiDirectionalLight*	sunDirectionalLight;	// smart ptr
	/*20*/ float				unk20;
	/*24*/ UInt8				unk24;
	/*25*/ UInt8				pad25[3];
};
STATIC_ASSERT(sizeof(Sun) == 0x28);

// 108
class Sky
{
public:
	// 08
	struct WeatherSelectionList
	{
		/*00*/ tList<TESWeather>	list;
	};
	STATIC_ASSERT(sizeof(WeatherSelectionList) == 0x8);

	enum
	{
		kFlagE0_Unk0 = 0,
		kFlagE0_Unk1 = 1,			// used with normal interiors
		kFlagE0_Unk2 = 2,			// used with interiors that behave like exteriors
		kFlagE0_Unk3 = 3,			// used with exteriors
		kFlagE0_Unk4 = 4,
	};

	enum
	{
		kColors_SkyUpper			= 0,
		kColors_Fog					= 1,
		kColors_CloudsLower			= 2,
		kColors_Ambient				= 3,
		kColors_Sunlight			= 4,
		kColors_Sun					= 5,
		kColors_Stars				= 6,
		kColors_SkyLower			= 7,
		kColors_Horizon				= 8,
		kColors_CloudsUpper			= 9,
	};

	// members
	//*000*/ void**					vtbl;
	/*004*/ NiNode*					skyNode;		// smart ptr
	/*008*/ NiNode*					moonsRootNode;	// smart ptr
	/*00C*/ NiAmbientLight*			unk00C;			// smart ptr
	/*010*/ TESClimate*				climate010;
	/*014*/ TESWeather*				weather014;
	/*018*/ UInt32					unk018;
	/*01C*/ TESWeather*				weather01C;
	/*020*/ UInt32					unk020;
	/*024*/ Atmosphere*				atmosphere;
	/*028*/ Stars*					stars;
	/*02C*/ Sun*					sun;
	/*030*/ Clouds*					clouds;
	/*034*/ Moon*					moonMasser;
	/*038*/ Moon*					moonSecunda;
	/*03C*/ Precipitation*			precipitation;
	/*040*/ Vector3					colors[10];
	/*0B8*/ Vector3					unk0B8;
	/*0C4*/ float					unk0C4;
	/*0C8*/ float					unk0C8;
	/*0CC*/ float					fogNearPlane;
	/*0D0*/ float					fogFarPlane;
	/*0D4*/ float					timeOfDay;
	/*0D8*/ float					unk0D8;
	/*0DC*/ float					unk0DC;
	/*0E0*/ UInt32					unk0E0;
	/*0E4*/ WeatherSelectionList*	currentWeatherSelection;
	/*0E8*/ float					unk0E8;
	/*0EC*/ UInt32					unk0EC;
	/*0F0*/ UInt32					unk0F0;
	/*0F4*/ float					unk0F4;
	/*0F8*/ float					unk0F8;
	/*0FC*/ UInt32					pad0FC;
	/*100*/ UInt32					flags100;
	/*104*/ UInt8					unk104;
	/*105*/ UInt8					pad105;


	virtual void Destroy(bool Free) = 0;

	static Sky**			Singleton;
};
STATIC_ASSERT(sizeof(Sky) == 0x108);