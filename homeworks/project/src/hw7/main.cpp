#include <Utopia/App/Editor/Editor.h>
#include <Utopia/Core/Components/Name.h>

#include <Utopia/App/Editor/InspectorRegistry.h>

#include <UECS/World.h>

#include "Components/DenoiseData.h"
#include "Systems/DenoiseSystem.h"

#ifndef NDEBUG
#include <dxgidebug.h>
#endif

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
        game->systemMngr.RegisterAndActivate<DenoiseSystem>();
        game->entityMngr.cmptTraits.Register<DenoiseData>();
        {
            auto [e, data, n] = game->entityMngr.Create<DenoiseData, Name>();
            n->value = "Denoise Data";
        }
        Ubpa::Utopia::InspectorRegistry::Instance().RegisterCmpts<DenoiseData>();

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
