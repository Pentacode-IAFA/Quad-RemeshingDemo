//
// Created by clement on 23/01/2022.
//

#ifndef PLUGINHOWTO_MAINWINDOW_H
#define PLUGINHOWTO_MAINWINDOW_H

#include <Gui/MainWindowInterface.hpp>
#include <Gui/RaGui.hpp>
#include <Gui/TreeModel/EntityTreeModel.hpp>

#include <QMainWindow>
#include <QDockWidget>

// include the KeyMappingManager
#include <Gui/Utils/KeyMappingManager.hpp>
#include <Gui/Utils/Keyboard.hpp>

// include Qt components
#include <QKeyEvent>

#include <Gui/Viewer/Viewer.hpp>

namespace Ra {
    namespace Gui {

/// This class manages most of the GUI of the application :
/// top menu, side toolbar and side dock.
        class RA_GUI_API MainWindow : public Ra::Gui::MainWindowInterface {

        Q_OBJECT

        public:
            /// Constructor and destructor.
            explicit MainWindow(uint w = 800, uint h = 640, QWidget *parent = nullptr);
            virtual ~MainWindow() override;

            Ra::Gui::Viewer* getViewer() override;

            Ra::Gui::SelectionManager* getSelectionManager() override;

            Ra::Gui::Timeline* getTimeline() override;

            void updateUi( Ra::Plugins::RadiumPluginInterface* plugin ) override;

            void onFrameComplete() override;

            void addRenderer( const std::string& name,
                              std::shared_ptr<Ra::Engine::Rendering::Renderer> e ) override;

            void print_name();
            void display_Patch( QKeyEvent* );

        public slots:

            void prepareDisplay() override;

            void cleanup() override;

            // Display help dialog about Viewer key-bindings
            void displayHelpDialog() override;

        signals:
            void frameUpdate();

            /// Emitted when the frame loads
            void fileLoading( const QString path );

        private slots:
            void loadFile();

            void clearMesh();

        private:
            void createConnections();
            Ra::Gui::KeyMappingManager::KeyMappingAction DISPLAY_PATCH;
            std::shared_ptr<Ra::Engine::Rendering::RenderObject> m_obj;
            std::array<Ra::Core::Utils::Color, 10> m_colors { Ra::Core::Utils::Color::Green(),
                                                              Ra::Core::Utils::Color::Blue(),
                                                              Ra::Core::Utils::Color::Yellow(),
                                                              Ra::Core::Utils::Color::Magenta(),
                                                              Ra::Core::Utils::Color::Cyan(),
                                                              Ra::Core::Utils::Color::White(),
                                                              Ra::Core::Utils::Color::Grey(),
                                                              Ra::Core::Utils::Color::Black(),
                                                              Ra::Core::Utils::Color::Skin(),
                                                              Ra::Core::Utils::Color::Red() };
            std::unique_ptr<Ra::Gui::Viewer> m_viewer;

            std::unique_ptr<Ra::Gui::SelectionManager> m_selectionManager;

            std::unique_ptr<Ra::Gui::ItemModel> m_sceneModel;

            QAction* loadFileAct;

            QAction* clearMeshAct;

            QDockWidget *m_dockWidget;
        };

    }
}
#endif //PLUGINHOWTO_MAINWINDOW_H
