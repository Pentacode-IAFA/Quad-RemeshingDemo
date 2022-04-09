//
// Created by clement on 23/01/2022.
//

#include <MainApplication.hpp>
#include <Gui/MainWindow.hpp>
#include "../libs/quadwild/components/field_computation/AutoRemesher.h"
#include "../libs/quadwild/components/field_computation/triangle_mesh_type.h"
#include "../libs/quadwild/components/field_computation/mesh_manager.h"

struct Parameters {
    Parameters() :
        remesh(true),
        sharpAngle(35),
        alpha(0.02),
        scaleFact(1),
        hasFeature(false),
        hasField(false)
    {

    }

    bool remesh;
    float sharpAngle;
    float alpha;
    float scaleFact;
    bool hasFeature;
    bool hasField;
};

inline void remeshAndField(
        FieldTriMesh& trimesh,
        const Parameters& parameters,
        const std::string& meshFilename,
        const std::string& sharpFilename,
        const std::string& fieldFilename)
{
    typename MeshPrepocess<FieldTriMesh>::BatchParam BPar;
    BPar.DoRemesh=parameters.remesh;
    BPar.feature_erode_dilate=4;
    BPar.remesher_aspect_ratio=0.3;
    BPar.remesher_iterations=15;
    BPar.remesher_termination_delta=10000;
    BPar.SharpFactor=6;
    BPar.sharp_feature_thr=parameters.sharpAngle;
    BPar.surf_dist_check=true;
    BPar.UpdateSharp=(!parameters.hasFeature);

    typename vcg::tri::FieldSmoother<FieldTriMesh>::SmoothParam FieldParam;
    FieldParam.alpha_curv=0.3;
    FieldParam.curv_thr=0.8;

    if (parameters.hasFeature) {
        bool loaded=trimesh.LoadSharpFeatures(sharpFilename);
        if (!loaded)
        {
            std::cout<<"ERROR: Wrong Sharp Feature File"<<std::endl;
            exit(0);
        }
        std::cout<<"Sharp Feature Length:"<<trimesh.SharpLenght()<<std::endl;
    }
    if (!parameters.hasField) {
        MeshPrepocess<FieldTriMesh>::BatchProcess(trimesh,BPar,FieldParam);
    }
    else {
        trimesh.LoadField(fieldFilename.c_str());
    }

    MeshPrepocess<FieldTriMesh>::SaveAllData(trimesh,meshFilename);
}

inline bool loadConfigFile(const std::string& filename, Parameters& parameters)
{
    FILE *f=fopen(filename.c_str(),"rt");

    if (f==NULL)return false;

    std::cout<<"READ CONFIG FILE"<<std::endl;

    int IntVar;
    fscanf(f,"do_remesh %d\n",&IntVar);
    if (IntVar==0)
        parameters.remesh=false;
    else
        parameters.remesh=true;

    fscanf(f,"sharp_feature_thr %f\n",&parameters.sharpAngle);

    fscanf(f,"alpha %f\n",&parameters.alpha);

    fscanf(f,"scaleFact %f\n",&parameters.scaleFact);

    fclose(f);

    std::cout << "Successful config import" << std::endl;

    return true;
}

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

    Parameters parameters;

    FieldTriMesh trimesh;

    std::string meshFilename= "/home/mafo/Downloads/300/300/Mechanical/gear.obj";
    std::string sharpFilename;
    std::string fieldFilename;

    //Use "." as decimal separator
    std::setlocale(LC_NUMERIC, "en_US.UTF-8");

    std::cout<<"Reading input..."<<std::endl;
    loadConfigFile("/home/mafo/dev/Quad-RemeshingDemo/libs/quadwild/components/field_computation/basic_setup.txt", parameters);

    for (int i=2;i<argc;i++)
    {
        int position;

        std::string pathTest=std::string(argv[i]);

        position=pathTest.find(".sharp");
        if (position!=-1)
        {
            sharpFilename=pathTest;
            parameters.hasFeature=true;
            continue;
        }

        position=pathTest.find(".txt");
        if (position!=-1)
        {
           loadConfigFile(pathTest.c_str(), parameters);
           continue;
        }

        position=pathTest.find(".rosy");
        if (position!=-1)
        {
           fieldFilename=pathTest;
           parameters.hasField=true;
           continue;
        }
    }


    std::cout<<"Loading:"<<meshFilename.c_str()<<std::endl;

    bool allQuad;
    bool loaded=trimesh.LoadTriMesh(meshFilename,allQuad);
    trimesh.UpdateDataStructures();

    if (!loaded)
    {
        std::cout<<"Wrong mesh filename"<<std::endl;
        exit(0);
    }

    std::cout<<"Loaded "<<trimesh.fn<<" faces and "<<trimesh.vn<<" vertices"<<std::endl;

    std::cout<<std::endl<<"--------------------- 1 - Remesh and field ---------------------"<<std::endl;
    remeshAndField(trimesh, parameters, meshFilename, sharpFilename, fieldFilename);
    
    return app.exec();
}


