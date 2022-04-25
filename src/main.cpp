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

#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Scene/EntityManager.hpp>
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


    //! [Creating the cube]
    auto cube = Ra::Core::Geometry::makeSharpBox( { 0.1f, 0.1f, 0.1f } );
    //! [Creating the cube]

    //! [Colorize the Cube]
    cube.addAttrib(
            "in_color",
            Ra::Core::Vector4Array { cube.vertices().size(), Ra::Core::Utils::Color::Blue() } );
    //! [Colorize the Cube]

    //! [Create the engine entity for the cube]
    auto e = app.m_engine->getEntityManager()->createEntity( "Green cube" );
    //! [Create the engine entity for the cube]

    //! [Create a geometry component with the cube]
    auto c = new Ra::Engine::Scene::TriangleMeshComponent( "Cube Mesh", e, std::move( cube ), nullptr );
    //! [Create a geometry component with the cube]

    //! [Register the entity/component association to the geometry system ]
    auto geometrySystem = app.m_engine->getSystem( "GeometrySystem" );
    geometrySystem->addComponent( e, c );

    app.m_mainWindow->prepareDisplay();
    return app.exec();
}




