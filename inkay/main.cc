#include "eg2/render/screen.hh"
#include "eg2/network/curl.hh"
#include "eg2/render/frames.hh"
#include "eg2/ax/stub.hh"
#include "inkay/render/gui.hh"
#include "inkay/repos/core.hh"

int main(void) {
    Render::Init();
    Network::Init();
    AX::Init();
    Inkay::GUI::Init();
    Inkay::Repos::Init();

    while (Render::IsRunning()) {
        Inkay::GUI::Update();

        Render::BeginFrame();
            Render::DRC(Inkay::GUI::Render);
            Render::TV(Inkay::GUI::Render);
        Render::EndFrame();
    }

    Inkay::Repos::Shutdown();
    Inkay::GUI::Shutdown();
    AX::Shutdown();
    Network::Shutdown();
    Render::Shutdown();
    return 0;
}