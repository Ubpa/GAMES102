#include <Utopia/App/Editor/Editor.h>
#include <Utopia/App/Editor/InspectorRegistry.h>

#include <UECS/World.h>

#include "Components/CanvasData.h"
#include "Systems/CanvasSystem.h"

//#define NDEBUG  // �رյ���״̬: #ifndef��#endif��#ifdef��#endif֮��Ĵ���ᱻ����

#ifndef NDEBUG  // Ĭ���ǿ�������״̬. 
#include <dxgidebug.h>
#endif

extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;  // https://www.cnblogs.com/sdragonx/p/12183194.html

using namespace Ubpa::Utopia;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	int rst;
    try {
        Editor app(hInstance);
        if(!app.Init())
            return 1;

        auto game = app.GetGameWorld();
        auto [canvasSystemIdx] = game->systemMngr.Register<CanvasSystem>();
        game->systemMngr.Activate(canvasSystemIdx);
        game->entityMngr.cmptTraits.Register<CanvasData>();
        game->entityMngr.Create<CanvasData>();

		rst = app.Run();
    }
    catch(Ubpa::UDX12::Util::Exception& e) {
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        rst = 1;
	}

#ifndef NDEBUG
	Microsoft::WRL::ComPtr<IDXGIDebug> debug;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug));
    if(debug)
	    debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
#endif

	return rst;
}
