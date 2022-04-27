//
// Created by clement on 23/01/2022.
//

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/ForwardRenderer.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Scene/Entity.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Gui/MainWindow.hpp>
#include <Gui/SelectionManager/SelectionManager.hpp>
#include <Gui/TreeModel/EntityTreeModel.hpp>
#include <Gui/Viewer/CameraManipulator.hpp>
#include <Gui/Viewer/Viewer.hpp>
#include <IO/AssimpLoader/AssimpFileLoader.hpp>
#include <MainApplication.hpp>
#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QMenuBar>

#include <fstream>

#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/GeometrySystem.hpp>

// include the render object interface to access object properties
#include <Core/Tasks/Task.hpp>

using namespace Ra;
using namespace Ra::Core;
using namespace Ra::Engine;
namespace Ra {
using namespace Gui;
using namespace Engine;
using namespace Engine::Rendering;

MainWindow::MainWindow(uint w, uint h, QWidget *parent)
    : MainWindowInterface(parent) {

  if (objectName().isEmpty())
    setObjectName(QString::fromUtf8("RadiumSimpleWindow"));
  resize(w, h);

  // Initialize the minimum tools for a Radium-Guibased Application
  m_viewer = std::make_unique<Viewer>();
  m_viewer->setObjectName(QStringLiteral("m_viewer"));

  // Initialize the scene interactive representation
  m_sceneModel = std::make_unique<Ra::Gui::ItemModel>(
      Ra::Engine::RadiumEngine::getInstance(), this);
  m_selectionManager =
      std::make_unique<Ra::Gui::SelectionManager>(m_sceneModel.get(), this);

  // initialize Gui for the application
  auto viewerWidget = QWidget::createWindowContainer(m_viewer.get());
  viewerWidget->setAutoFillBackground(false);
  setCentralWidget(viewerWidget);
  setWindowTitle(QString("Radium player"));
  auto fileMenu = menuBar()->addMenu(tr("&File"));
  loadFileAct = new QAction("open");
  clearMeshAct = new QAction("clear mesh");
  fileMenu->addAction(loadFileAct);
  fileMenu->addAction(clearMeshAct);


  // Plugin widget
  QDockWidget *dockWidget = new QDockWidget("Dock", this);
  dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
  m_dockWidget = dockWidget;

    DISPLAY_BASE= getViewer()->addKeyPressEventAction(
            "DISPLAY_BASE", "Key_A", "", "", "false", [this]( QKeyEvent* event ) {
                std::cout << "A" << std::endl;
                display_file(event,"twirl20k.obj");
            } );

    DISPLAY_REM = getViewer()->addKeyPressEventAction(
            "DISPLAY_REM", "Key_Z", "", "", "false", [this]( QKeyEvent* event ) {
                std::cout << "Z" << std::endl;
                display_file(event,"twirl20k_rem.obj");
            } );
    DISPLAY_PATCH = getViewer()->addKeyPressEventAction(
            "DISPLAY_PATCH", "Key_E", "", "", "false", [this](QKeyEvent *event) {
                std::cout << "E" << std::endl;
                this->display_patch(event);
            });
    DISPLAY_QUADS = getViewer()->addKeyPressEventAction(
            "DISPLAY_QUADS", "Key_R", "", "", "false", [this]( QKeyEvent* event ) {
                std::cout << "R" << std::endl;

                display_file(event,"twirl20k_quadrangulation.obj");
            } );
    DISPLAY_QUADS_SMOOTH = getViewer()->addKeyPressEventAction(
            "DISPLAY_QUADS_SMMOTH", "Key_T", "", "", "false", [this]( QKeyEvent* event ) {
                std::cout << "T" << std::endl;

                display_file(event,"twirl20k_quadrangulation_smooth.obj");
            } );

  createConnections();
}

MainWindow::~MainWindow() = default;

Ra::Gui::Viewer *MainWindow::getViewer() { return m_viewer.get(); }

Ra::Gui::SelectionManager *MainWindow::getSelectionManager() {
  return m_selectionManager.get();
}

Ra::Gui::Timeline *MainWindow::getTimeline() { return nullptr; }

void MainWindow::updateUi(Ra::Plugins::RadiumPluginInterface *plugin) {
  QString name;
  if (plugin->doAddWidget(name)) {
    m_dockWidget->setWidget(plugin->getWidget());
  }
}
void MainWindow::onFrameComplete() {}
void MainWindow::addRenderer(
    const std::string &, std::shared_ptr<Ra::Engine::Rendering::Renderer> e) {
  e->enableDebugDraw(false);
  m_viewer->addRenderer(e);
}

void MainWindow::prepareDisplay() {
  m_selectionManager->clear();
  if (m_viewer->prepareDisplay()) {
    emit frameUpdate();
  }
}

void MainWindow::cleanup() { m_viewer.reset(nullptr); }

void MainWindow::loadFile() {

  QString filter;

  QString allexts;
  for (const auto &loader : mainApp->m_engine->getFileLoaders()) {
    QString exts;
    for (const auto &e : loader->getFileExtensions()) {
      exts.append(QString::fromStdString(e) + tr(" "));
    }
    allexts.append(exts + tr(" "));
    filter.append(QString::fromStdString(loader->name()) + tr(" (") + exts +
                  tr(");;"));
  }
  // add a filter concetenatting all the supported extensions
  filter.prepend(tr("Supported files (") + allexts + tr(");;"));

  // remove the last ";;" of the string
  filter.remove(filter.size() - 2, 2);

  QSettings settings;
  QString path = settings.value("files/load", QDir::homePath()).toString();
  QStringList pathList =
      QFileDialog::getOpenFileNames(this, "Open Files", path, filter);

  if (!pathList.empty()) {
    settings.setValue("files/load", pathList.front());

    for (const auto &file : pathList) {
      emit fileLoading(file);
    }
  }
}

void MainWindow::clearMesh() {
    auto engine = Ra::Engine::RadiumEngine::getInstance();
    engine->getEntityManager()->deleteEntities();
}


// -- Fonctions affichages remaillages

void MainWindow::display_patch(QKeyEvent *) {
  auto engine = Ra::Engine::RadiumEngine::getInstance();
  this->clearMesh();
  engine->loadFile("../../src/Assets/twirl20k_p0.obj");
  auto entitie = engine->getEntityManager()->getEntities()[1];

  auto c = entitie->getComponents()[0].get();
  auto ro = Ra::Engine::RadiumEngine::getInstance()
                ->getRenderObjectManager()
                ->getRenderObject(c->m_renderObjects[0]);

  auto &mesh = dynamic_cast<Ra::Core::Geometry::TriangleMesh &>(
      ro->getMesh()->getAbstractGeometry());

  auto new_mesh = Ra::Core::Geometry::TriangleMesh(mesh);

  new_mesh.addAttrib("in_color",
                     Ra::Core::Vector4Array{new_mesh.vertices().size(), Ra::Core::Utils::Color::White()});

  auto &attrib =
      new_mesh.getAttrib(new_mesh.getAttribHandle<Vector4>("in_color"));

  auto &cont = attrib.getDataWithLock();

  int i = 0;
  std::ifstream stream("../../src/Assets/twirl20k_p0.patch");
  if(stream.is_open()){
      std::cout << "Fichier ouvert" << std::endl;
  }
  else{
      std::cout << "Echec ouverture fichier" << std::endl;
  }
  std::string line;
  std::getline(stream, line);
  while (std::getline(stream, line)) {
    for(int j = 0; j < 3; j++) {
        cont[i+j] = this->m_colors[std::atoi(line.c_str()) % m_colors.size()];
    }
    i+=3;
  }

  attrib.unlock();
  auto e = engine->getEntityManager()->createEntity("Patch");
  auto c_final = new Ra::Engine::Scene::TriangleMeshComponent(
      "Patch Mesh", e, std::move(new_mesh), nullptr);
  auto geometrySystem = engine->getSystem("GeometrySystem");
  geometrySystem->addComponent(e, c_final);

  engine->getEntityManager()->removeEntity(entitie);
  this->prepareDisplay();
}

void MainWindow::display_file(QKeyEvent *, std::string file){
    auto engine = Ra::Engine::RadiumEngine::getInstance();

    this->clearMesh();
    engine->loadFile("../../src/Assets/"+file);

    this->prepareDisplay();
}

void MainWindow::createConnections() {
  connect(loadFileAct, &QAction::triggered, this, &MainWindow::loadFile);
  // Loading setup.
  connect(this, &MainWindow::fileLoading, mainApp,
          &Ra::Gui::BaseApplication::loadFile);
  // connect( this, &MainWindow::fileLoading, this, &MainWindow::print_name);

  connect(clearMeshAct, &QAction::triggered, this, &MainWindow::clearMesh);
}

void MainWindow::displayHelpDialog() { m_viewer->displayHelpDialog(); }

} // namespace Ra
