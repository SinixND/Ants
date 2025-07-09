#include "App.h"
#include "AppConfigs.h"

#include <raygui.h>

int main( /* int argc, char** argv */ )
{
    App app{};

    app.init();
    app.run();
    app.deinit();

    return 0;
}

