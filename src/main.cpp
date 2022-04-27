#include <MainApplication.hpp>
#include <Gui/MainWindow.hpp>

#include <Gui/Viewer/Viewer.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/Entity.hpp>

// include the KeyMappingManager

// include Qt components
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/GeometrySystem.hpp>

#include <QTimer>


class MainWindowFactory : public Ra::Gui::BaseApplication::WindowFactory
{
public:
    using Ra::Gui::BaseApplication::WindowFactory::WindowFactory;
    Ra::Gui::MainWindowInterface* createMainWindow() const override {
        return new Ra::Gui::MainWindow();
    }
};

int main( int argc, char** argv ) {
    Ra::MainApplication app( argc, argv );
    app.initialize( MainWindowFactory() );
    app.setContinuousUpdate( true );

    auto engine = Ra::Engine::RadiumEngine::getInstance();

    // chargement de la mesh de base
    engine->loadFile("../../src/Assets/twirl20k.obj");
    app.m_mainWindow->prepareDisplay();
    return app.exec();
}




