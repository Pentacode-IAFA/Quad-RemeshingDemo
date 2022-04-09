//
// Created by clement on 23/01/2022.
//

#include <MainApplication.hpp>
#include <Gui/MainWindow.hpp>
#include "../libs/quadwild/components/field_computation/AutoRemesher.h"
#include "../libs/quadwild/components/field_computation/triangle_mesh_type.h"

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
    app.setContinuousUpdate( false );

    FieldTriMesh tri_mesh;
    
    return app.exec();
}


