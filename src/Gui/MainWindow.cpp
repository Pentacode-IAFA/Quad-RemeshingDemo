//
// Created by clement on 23/01/2022.
//

#include <Gui/MainWindow.hpp>
#include <Engine/Rendering/ForwardRenderer.hpp>
#include <Gui/SelectionManager/SelectionManager.hpp>
#include <Gui/TreeModel/EntityTreeModel.hpp>
#include <Gui/Viewer/CameraManipulator.hpp>
#include <Gui/Viewer/Viewer.hpp>

namespace Ra{
    using namespace Gui;
    using namespace Engine;
    using namespace Engine::Rendering;

    MainWindow::MainWindow(QWidget *parent) : MainWindowInterface(parent) {

        if ( objectName().isEmpty() ) setObjectName( QString::fromUtf8( "RadiumSimpleWindow" ) );

        // Initialize the minimum tools for a Radium-Guibased Application
        m_viewer = std::make_unique<Viewer>();
        m_viewer->setObjectName( QStringLiteral( "m_viewer" ) );

        // Initialize the scene interactive representation
        m_sceneModel =
                std::make_unique<Ra::Gui::ItemModel>( Ra::Engine::RadiumEngine::getInstance(), this );
        m_selectionManager = std::make_unique<Ra::Gui::SelectionManager>( m_sceneModel.get(), this );

        // initialize Gui for the application
        auto viewerWidget = QWidget::createWindowContainer( m_viewer.get() );
        viewerWidget->setAutoFillBackground( false );
        setCentralWidget( viewerWidget );
        setWindowTitle( QString( "Radium player" ) );

        createConnections();
    }

    MainWindow::~MainWindow() = default;

    Ra::Gui::Viewer* MainWindow::getViewer() {
        return m_viewer.get();
    }

    Ra::Gui::SelectionManager* MainWindow::getSelectionManager() {
        return m_selectionManager.get();
    }

    Ra::Gui::Timeline* MainWindow::getTimeline() {
        return nullptr;
    }

    void MainWindow::updateUi(Ra::Plugins::RadiumPluginInterface*) {}
    void MainWindow::onFrameComplete() {}
    void MainWindow::addRenderer(const std::string&, std::shared_ptr<Ra::Engine::Rendering::Renderer> e)
    {
        e->enableDebugDraw(false);
        m_viewer->addRenderer( e );
    }

    void MainWindow::prepareDisplay()
    {
        m_selectionManager->clear();
        if ( m_viewer->prepareDisplay() )
        {
            emit frameUpdate();
        }
    }

    void MainWindow::cleanup()
    {
        m_viewer.reset( nullptr );
    }

    void MainWindow::createConnections() {}

    void MainWindow::displayHelpDialog() {
        m_viewer->displayHelpDialog();
    }
}