#include "App.h"

#include "AppConfigs.h"
#include "AppData.h"
#include "ColorData.h"
#include "EventDispatcher.h"
#include "EventId.h"
#include <raylib.h>

#if defined( EMSCRIPTEN )
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

void setupRaylib( AppConfig const& config )
{
#if !defined( NOGUI )
    //* Raylib flags
    SetConfigFlags( FLAG_WINDOW_RESIZABLE );

    if ( config.vSync )
    {
        SetConfigFlags( FLAG_VSYNC_HINT );
    }

    //* Initialize window
    InitWindow(
        config.windowWidth,
        config.windowHeight,
        "Title"
    );

    //* Raylib Settings
    SetWindowIcon( AppData::FAVICON );

#if defined( EMSCRIPTEN )
    MaximizeWindow();
#endif
#endif

    SetTargetFPS( AppData::FPS_TARGET );

    SetExitKey( AppData::EXIT_KEY );
}

void App::init()
{
    setupRaylib( config );

    setupAppEvents();

    simulation.init();
}

void updateFullscreenState()
{
#if !defined( NOGUI )
    if ( IsKeyPressed( KEY_F11 ) )
    {
        if ( IsWindowMaximized() )
        {
            RestoreWindow();
        }

        else
        {
            MaximizeWindow();
        }
    }

    if ( IsWindowResized() )
    {
        snx::EventDispatcher::notify( EventId::WINDOW_RESIZED );
    }
#endif
}

/// @brief Void argument in function signature needed for emscripten
void updateApp( void* arg )
{
    App& app = *(App*)arg;

    updateFullscreenState();

    app.screenWidth = { GetScreenWidth() };
    app.screenHeight = { GetScreenHeight() };

    app.mousePosition = { 0, 0 };
    if ( IsMouseButtonDown( MOUSE_LEFT_BUTTON ) )
    {
        app.mousePosition = GetMousePosition();
    }

    app.dt = GetFrameTime();

    // app.project.update();

    app.render();
}

void App::run()
{
#if defined( EMSCRIPTEN )
    emscripten_set_main_loop_arg(
        updateApp,
        this,
        AppData::FPS_TARGET /*FPS*/,
        1 /*Simulate infinite loop*/
    );
#elif defined( NOGUI )
    while ( !( IsKeyDown( AppData::EXIT_KEY ) ) )
    {
        updateApp( this );
    }
#else
    while ( !( WindowShouldClose() ) )
    {
        updateApp( this );
    }
#endif
}

void App::render()
{
    BeginDrawing();
    ClearBackground( ColorData::BG );

    DrawFPS( 0, 0 );

    EndDrawing();
}

void App::deinit()
{
    simulation.deinit();

    //* Close window and opengl context
    CloseWindow();
}

void App::setupAppEvents()
{
    snx::EventDispatcher::addListener(
        EventId::WINDOW_RESIZED,
        [&]()
        {
            simulation.init();
        },
        false
    );
}

