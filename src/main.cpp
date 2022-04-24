#include <MainApplication.hpp>
#include <Gui/MainWindow.hpp>
#include <QKeyEvent>

#include <Gui/Viewer/Viewer.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/Entity.hpp>

// include the KeyMappingManager
#include <Gui/Utils/KeyMappingManager.hpp>
#include <Gui/Utils/Keyboard.hpp>

// include Qt components
#include <QKeyEvent>
#include <Gui/Viewer/Viewer.hpp>

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

    return app.exec();
}


