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

        private:
            void createConnections();

            std::unique_ptr<Ra::Gui::Viewer> m_viewer;

            std::unique_ptr<Ra::Gui::SelectionManager> m_selectionManager;

            std::unique_ptr<Ra::Gui::ItemModel> m_sceneModel;

            QAction* loadFileAct;

            QDockWidget *m_dockWidget;
        };

    }
}
#endif //PLUGINHOWTO_MAINWINDOW_H
