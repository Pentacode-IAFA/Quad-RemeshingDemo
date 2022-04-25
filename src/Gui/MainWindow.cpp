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
  fileMenu->addAction(loadFileAct);

  // Plugin widget
  QDockWidget *dockWidget = new QDockWidget("Dock", this);
  dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
  m_dockWidget = dockWidget;

  // Configure Key Event
  DISPLAY_PATCH = getViewer()->addKeyPressEventAction(
      "DISPLAY_PATCH", "Key_U", "", "", "false", [this](QKeyEvent *event) {
        std::cout << "U"
                  << "\n";
        this->display_Patch(event);
      });

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

void MainWindow::display_Patch(QKeyEvent *) {
  auto engine = Ra::Engine::RadiumEngine::getInstance();
  auto entitie = engine->getEntityManager()->getEntities()[1];

  auto c = entitie->getComponents()[0].get();
  auto ro = Ra::Engine::RadiumEngine::getInstance()
                ->getRenderObjectManager()
                ->getRenderObject(c->m_renderObjects[0]);

  auto &mesh = dynamic_cast<Ra::Core::Geometry::TriangleMesh &>(
      ro->getMesh()->getAbstractGeometry());

  auto new_mesh = Ra::Core::Geometry::TriangleMesh(mesh);

  new_mesh.addAttrib("in_color",
                     Ra::Core::Vector4Array{new_mesh.vertices().size(),
                                            Ra::Core::Utils::Color::Green()});

  auto &attrib =
      new_mesh.getAttrib(new_mesh.getAttribHandle<Vector4>("in_color"));

  auto &cont = attrib.getDataWithLock();

  //   std::for_each(cont.begin(), cont.end(), [](Vector4 &v) {
  //     v.x() = float(rand()) / float(RAND_MAX);
  //     v.y() = float(rand()) / float(RAND_MAX);
  //     v.z() = float(rand()) / float(RAND_MAX);
  //   });

  std::ofstream file("../src/Assets/bucket_p0.patch");

  //   for (int i = 0; i < cont.size(); i++) {
  //     cont[i].x() = float(rand()) / float(RAND_MAX);
  //     cont[i].y() = float(rand()) / float(RAND_MAX);
  //     cont[i].z() = float(rand()) / float(RAND_MAX);

  //   }

  int i = 0;
  std::ifstream stream("../src/Assets/bucket_p0.patch");

  std::stringstream ss;
  std::string line;
  //   std::getline(stream, line);
  while (std::getline(stream, line)) {
    ss << line << '\n';
    cont[i].x() = this->m_colors[std::atoi(line.c_str()) % m_colors.size()].x();
    cont[i].y() = this->m_colors[std::atoi(line.c_str()) % m_colors.size()].y();
    cont[i].z() = this->m_colors[std::atoi(line.c_str()) % m_colors.size()].z();
    i++;
    printf("%s\n", line.c_str());
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

void MainWindow::print_name() {
  auto engine = Ra::Engine::RadiumEngine::getInstance();
  auto manager = engine->getRenderObjectManager();
  auto entities = engine->getEntityManager()->getEntities();

  for (int i = 1; i < entities.size(); i++) {

    auto e = entities[i];
    auto ros = e->getComponents()[0]->getRenderObjects();
    std::cout << ros.size() << "\n";

    for (int y = 0; y < ros.size(); y++) {
      auto &miniros = ros[y];
      auto renderO = manager->getRenderObject(miniros);

      auto &mesh = dynamic_cast<Ra::Core::Geometry::TriangleMesh &>(
          renderO->getMesh()->getAbstractGeometry());
      std::cout << "test : " << mesh.getIndices().size() << "\n";
      mesh.colorize(Ra::Core::Utils::Color::Green());
    }

    Transform transform(Translation{10, 1, 0});
    e->setTransform(transform);
  }
}

void MainWindow::createConnections() {
  connect(loadFileAct, &QAction::triggered, this, &MainWindow::loadFile);
  // Loading setup.
  connect(this, &MainWindow::fileLoading, mainApp,
          &Ra::Gui::BaseApplication::loadFile);
  // connect( this, &MainWindow::fileLoading, this, &MainWindow::print_name);
}

void MainWindow::displayHelpDialog() { m_viewer->displayHelpDialog(); }
} // namespace Ra