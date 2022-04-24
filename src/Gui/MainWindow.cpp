//
// Created by clement on 23/01/2022.
//

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Engine/Data/Mesh.hpp>
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
    printf("plop\n");
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

void MainWindow::print_name() {
  auto engine = Ra::Engine::RadiumEngine::getInstance();
  auto manager = engine->getRenderObjectManager();
  auto entities = engine->getEntityManager()->getEntities();

  for (int i = 1; i < entities.size(); i++) {
    auto e = entities[i];

    std::cout << e->getName() << "\n";
    auto &ros = e->getComponents()[0]->getRenderObjects();
    std::cout << ros.size() << "\n";

    for (int i = 0; i < e->getComponents().size(); i++) {
      std::cout << e->getComponents()[i]->getName() << std::endl;
    }

    for (int y = 0; y < ros.size(); y++) {
      auto &miniros = ros[y];
      auto renderO = manager->getRenderObject(miniros);
      auto &mesh = dynamic_cast<Ra::Core::Geometry::TriangleMesh &>(
          renderO->getMesh()->getAbstractGeometry());
      std::cout << "tset " << mesh.getIndices().size() << "\n";

      // mesh.vertexAttribs().addAttrib<Core::Vector4>("in_color");

      // const std::string attribName = "in_color";
      // auto attribHandle = mesh.getAttribHandle<Core::Vector4>("in_color");

      // mesh.vertexAttribs().for_each_attrib([](Ra::Core::Utils::AttribBase *a)
      // {
      //   std::cout << a->getName() << std::endl;
      // });

      // std::cout << "plop2" << std::endl;
      // auto &attrib = mesh.vertexAttribs().getAttrib(attribHandle);

      // Ra::Core::Vector3Array data;
      // data.resize(attrib.getSize());

      // // std::for_each(data.begin(), data.end(), [](Ra::Core::Vector3 v) {
      // //   v.x() = float(rand()) / float(RAND_MAX);
      // //   v.y() = float(rand()) / float(RAND_MAX);
      // //   v.z() = float(rand()) / float(RAND_MAX);
      // // });

      // for (int i = 0; i < data.size(); i++) {
      //   data[i].x() = float(rand()) / float(RAND_MAX);
      //   data[i].y() = float(rand()) / float(RAND_MAX);
      //   data[i].z() = float(rand()) / float(RAND_MAX);
      // }

      mesh.colorize(Ra::Core::Utils::Color::Green());

      // attrib.setData(std::move(data));
      // attrib.unlock();
    }
  };
}

void MainWindow::createConnections() {
  connect(loadFileAct, &QAction::triggered, this, &MainWindow::loadFile);
  // Loading setup.
  connect(this, &MainWindow::fileLoading, mainApp,
          &Ra::Gui::BaseApplication::loadFile);
  connect(this, &MainWindow::fileLoading, this, &MainWindow::print_name);
}

void MainWindow::displayHelpDialog() { m_viewer->displayHelpDialog(); }
} // namespace Ra