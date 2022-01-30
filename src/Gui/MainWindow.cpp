//
// Created by clement on 23/01/2022.
//

#include <MainApplication.hpp>
#include <Gui/MainWindow.hpp>
#include <Engine/Rendering/ForwardRenderer.hpp>
#include <Gui/SelectionManager/SelectionManager.hpp>
#include <Gui/TreeModel/EntityTreeModel.hpp>
#include <Gui/Viewer/CameraManipulator.hpp>
#include <Gui/Viewer/Viewer.hpp>
#include <IO/AssimpLoader/AssimpFileLoader.hpp>
#include <Core/Asset/FileData.hpp>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>

namespace Ra{
    using namespace Gui;
    using namespace Engine;
    using namespace Engine::Rendering;

    MainWindow::MainWindow(uint w, uint h, QWidget *parent) : MainWindowInterface(parent) {

        if ( objectName().isEmpty() ) setObjectName( QString::fromUtf8( "RadiumSimpleWindow" ) );
        resize( w, h );

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
        auto fileMenu = menuBar()->addMenu(tr("&File"));
        loadFileAct = new QAction("open");
        fileMenu->addAction(loadFileAct);

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

    void MainWindow::loadFile() {

        QString filter;

        QString allexts;
        for ( const auto& loader : mainApp->m_engine->getFileLoaders() )
        {
            QString exts;
            for ( const auto& e : loader->getFileExtensions() )
            {
                exts.append( QString::fromStdString( e ) + tr( " " ) );
            }
            allexts.append( exts + tr( " " ) );
            filter.append( QString::fromStdString( loader->name() ) + tr( " (" ) + exts + tr( ");;" ) );
        }
        // add a filter concetenatting all the supported extensions
        filter.prepend( tr( "Supported files (" ) + allexts + tr( ");;" ) );

        // remove the last ";;" of the string
        filter.remove( filter.size() - 2, 2 );

        QSettings settings;
        QString path         = settings.value( "files/load", QDir::homePath() ).toString();
        QStringList pathList = QFileDialog::getOpenFileNames( this, "Open Files", path, filter );

        if ( !pathList.empty() )
        {
            settings.setValue( "files/load", pathList.front() );

            for ( const auto& file : pathList )
            {
                emit fileLoading( file );
            }
        }
    }


    void MainWindow::createConnections() {
        connect(loadFileAct, &QAction::triggered, this, &MainWindow::loadFile);
        // Loading setup.
        connect( this, &MainWindow::fileLoading, mainApp, &Ra::Gui::BaseApplication::loadFile );
    }

    void MainWindow::displayHelpDialog() {
        m_viewer->displayHelpDialog();
    }
}